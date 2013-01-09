@ Copyright (C) 2011 Dave Griffiths
@
@ This program is free software; you can redistribute it and/or modify
@ it under the terms of the GNU General Public License as published by
@ the Free Software Foundation; either version 2 of the License, or
@ (at your option) any later version.
@
@ This program is distributed in the hope that it will be useful,
@ but WITHOUT ANY WARRANTY; without even the implied warranty of
@ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
@ GNU General Public License for more details.
@
@ You should have received a copy of the GNU General Public License
@ along with this program; if not, write to the Free Software
@ Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
@ ----------------------------------------------------
@ an arm9 synth for betablocker ds
@ ----------------------------------------------------
        .text
	    .align  2
@ ----------------------------------------------------
@ constants
@ ----------------------------------------------------
.audio_level:
        .word   0x0003FFF          
.tablength:
        .word   0x00003FF @ IN BYTES         
@ ----------------------------------------------------
@ qrz_white_noise: *dst, clock, length, freq
@ ----------------------------------------------------
        .global qrz_white_noise
	    .type   qrz_white_noise, %function
qrz_white_noise:
        push    {r4,r5,r6}
        mov     r4, r1              @ r4=randstate (load with clock)
        ldr     r5, .rnd_data       @ load the multiplier
        ldr     r6, .rnd_data+1     @ load the addition 
.noise_loop:
        mla     r4, r5, r4, r6      @ r4 = (r6 + (r5 * r4)) 
        strh    r4, [r0], #2        @ *dst = clock dst++
        subs    r2, r2, #1          @ length--
        bne     .noise_loop         @ repeat until length zero
        pop     {r4,r5,r6}
        bx      lr                  @ return
.rnd_data:
        .word   0x000343FD          @ nicked from ansi c rand()
        .word   0x00269EC3
@ ----------------------------------------------------
@ qrz_tabread: *dst, length, clk, freq, [*tab]
@ ----------------------------------------------------
        .global qrz_tabread
	    .type   qrz_tabread, %function
qrz_tabread:
        push    {r4,r5,r6}
        ldr     r4, [r13,#12]       @ load *tab into r4 
        ldr     r6, .tablength
.tabread_loop:
        mov     r5, r2, lsr #4      @ scale down pos into r5
        and     r5, r5, r6          @ mask to clamp in range
        ldrh    r5, [r4,r5]         @ load the sample into r5
        strh    r5, [r0], #2        @ *dst=r5 dst++
        add     r2, r2, r3          @ clock+=freq
        subs    r1, r1, #1          @ length--
        bne     .tabread_loop      @ repeat until length zero
        mov     r0, r2
        pop     {r4,r5,r6}   
        bx      lr                  @ return
@ ----------------------------------------------------
@ qrz_tabread_lfo: *dst, length, clk, freq, [*tab]
@ same as tabread but slower
@ ----------------------------------------------------
        .global qrz_tabread_lfo
	    .type   qrz_tabread_lfo, %function
qrz_tabread_lfo:
        push    {r4,r5,r6}
        ldr     r4, [r13,#12]       @ load *tab into r4 
        ldr     r6, .tablength
.tabread_lfo_loop:
        mov     r5, r2, lsr #8      @ slow down the clock
        and     r5, r5, r6          @ mask to clamp in range
        ldrh    r5, [r4,r5]         @ load the sample into r5
        strh    r5, [r0], #2        @ *dst=r5 dst++
        add     r2, r2, r3          @ clock+=freq
        subs    r1, r1, #1          @ length--
        bne     .tabread_lfo_loop   @ repeat until length zero
        mov     r0, r2
        pop     {r4,r5,r6}   
        bx      lr                  @ return
@ ----------------------------------------------------
@ qrz_tabread_env: *dst, length, clk, speed, [*tab]
@ even slower than lfo, and zeros off the end
@ ----------------------------------------------------
        .global qrz_tabread_env
	    .type   qrz_tabread_env, %function
qrz_tabread_env:
        push    {r4,r5,r6}
        ldr     r4, [r13,#12]       @ load *tab into r4 
        ldr     r6, .tablength
.tabread_env_loop:
        mov     r5, r2, asr #9      @ scale the clock down
        cmp     r5, r6              @ compare with tablength
        bge     .tabread_env_runoff @ if we've run off the end
        ldrh    r5, [r4,r5]         @ load the sample into r5
        strh    r5, [r0], #2        @ *dst=r5 dst++
        add     r2, r2, r3          @ clock+=freq
        subs    r1, r1, #1          @ length--
        bne     .tabread_env_loop   @ repeat until length zero
        mov     r0, r2
        pop     {r4,r5,r6}   
        bx      lr                  @ return
.tabread_env_runoff:
        mov     r5, #0              @ fill with zeros
.tabread_env_runoff_loop:
        strh    r5, [r0], #2        @ *dst=r5 dst++
        subs    r1, r1, #1          @ length--
        bne     .tabread_env_runoff_loop @ repeat until length zero
        mov     r0, r2
        pop     {r4,r5,r6}   
        bx      lr                  @ return
@ ----------------------------------------------------
@ qrz_tabread_mod: *dst, length, clk, *freq, [*tab]
@ ----------------------------------------------------
        .global qrz_tabread_mod
	    .type   qrz_tabread_mod, %function
qrz_tabread_mod:
        push    {r4,r5,r6}
        ldr     r4, [r13,#12]       @ load *tab into r4 
        ldr     r6, .tablength
.tabread_mod_loop:
        mov     r5, r2, asr #8      @ scale down speed
        and     r5, r5, r6          @ mask to clamp in range
        ldrh    r5, [r4,r5]         @ load the sample into r5
        strh    r5, [r0], #2        @ *dst=r5 dst++
        ldrh    r5, [r3], #2        @ read freq into r5, freq++
        add     r2, r2, r5          @ clock+=freq
        subs    r1, r1, #1          @ length--
        bne     .tabread_mod_loop   @ repeat until length zero
        mov     r0, r2
        pop     {r4,r5,r6}   
        bx      lr                  @ return
@ ----------------------------------------------------
@ qrz_tabwrite: *src, length, clk, *tab
@ ----------------------------------------------------
        .global qrz_tabread
	    .type   qrz_tabread, %function
qrz_tabwrite:
        push    {r4,r5}
        ldr     r5, .tablength
.tabwrite_loop:
        ldrh    r4, [r0], #2        @ load the sample into r5
        strh    r4, [r3], #2        @ *dst=r5 dst++
        add     r2, r2, #2          @ clock++
        and     r2, r2, r5          @ mask to clamp in range
        subs    r1, r1, #1          @ length--
        bne     .tabwrite_loop      @ repeat until length zero
        mov     r0, r2
        pop     {r4,r5}   
        bx      lr                  @ return
@ ----------------------------------------------------
@ qrz_maximus: *dst, length, readpos, writepos [freq, *tab]
@ ----------------------------------------------------
        .global qrz_maximus_inner
	    .type   qrz_maximus_inner, %function
qrz_maximus_inner:
        push    {r4,r5,r6,r7,r8}    @ push the registers we need
        ldr     r4, [r13,#20]       @ load freq from stack into r4 
        ldr     r5, [r13,#24]       @ load *tab from stack into r5 
        ldr     r6, .tablength      @ load the tablen into r6
.maximus_loop:
        mov     r8, r2, lsr #7
        and     r8, r8, r6          @ mask readpos into range
        ldrh    r7, [r5,r8]         @ load the sample into r7
        strh    r7, [r0], #2        @ write output: *dst=r7 dst++
        strh    r7, [r5,r3]         @ feedback into tab[writepos]=r7 
        add     r2, r2, r4          @ readpos+=freq
        add     r3, r3, #2          @ writepos++
        and     r3, r3, r6          @ mask writepos into range
        subs    r1, r1, #1          @ length--
        bne     .maximus_loop       @ repeat until length zero
        mov     r0, r2              @ return readpos
        pop     {r4,r5,r6,r7,r8}   
        bx      lr                  @ return
@ ----------------------------------------------------
@ qrz_thrsh: *dst, length, *src, *thr
@ ----------------------------------------------------
        .global qrz_thrsh
	    .type   qrz_thrsh, %function
qrz_thrsh:
        push    {r4,r5,r6,r7}
        ldr     r4, .audio_level
.thrsh_loop:
        ldrh    r5, [r2], #2        @ load the sample into r5
        ldrh    r6, [r4], #2        @ load the thr sample into r6
        mov     r7, r4
        cmp     r5, r6              @ compare sample with thr
        rsblt   r7, r7, #0          @ r5 = -r6 if less than
        strh    r7, [r0], #2        @ *dst=clock dst++
        subs    r1, r1, #1          @ length--
        bne     .thrsh_loop         @ repeat until length zero
        pop     {r4,r5,r6,r7}
        bx      lr                  @ return




        
        

        