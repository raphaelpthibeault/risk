; implementation of atomic operations defined in atomic.h

; sets rax to 1 if the carry flag is clear (CF = 0) and to 0 if the carry flag is set (CF = 1)
%macro	CF_RESULT 0
	mov		rcx, 1
	mov		rax, 0
	cmovnc	rax, rcx	; conditional mov if not carry
%endmacro

GLOBAL	atomic_lock
atomic_lock:	; convention: rdi=[mutex], rsi=[bit]
	lock bts	QWORD [rdi], rsi
	CF_RESULT		
	ret

GLOBAL	atomic_unlock
atomic_unlock:	; convention: rdi=[mutex], rsi=[bit]
	btr		QWORD [rdi], rsi
	CF_RESULT
	ret

GLOBAL	atomic_spinlock
atomic_spinlock:	; convention: rdi=[mutex], rsi=[bit]
	.acquire:
		lock bts	QWORD [rdi], rsi
		jnc			.exit				; CF = 0 to begin with
	.spin:
		pause
		bt			QWORD [rdi], rsi
		jc			.spin				; CF = 1 still
		jmp			.acquire
	.exit:
		ret

GLOBAL	atomic_accumulate
atomic_accumulate:	; rdi=[accumulator], rsi=[amount]
	lock xadd	QWORD [rdi], rsi
	mov			rax, rsi
	ret

