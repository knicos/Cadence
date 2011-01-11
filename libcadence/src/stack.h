
#ifndef _doste_STACK_
#define _doste_STACK_

namespace cadence {

	#ifdef X86_64
	typedef unsigned long long stacksize_t;
	#else
	typedef unsigned int stacksize_t;
	#endif

	/**
	 * This stack class represents the CPU stack for individual threads. It
	 * enables task switching but letting you gain access to the stack of
	 * any thread and controlling its contents.
	 */
	class Stack {
		public:
		Stack(int size=1024);
		~Stack();

		stacksize_t stackPointer() { return m_sp; }

		void push(stacksize_t element);
		stacksize_t pop();
	};
};

#endif
