# uenux2 api::CThread::~CThread (target fn 2721, CThread vtable @1599952 slot 0) in src/uenux2/src/api/ipc/cthread.cpp:
# the file the stub's link-order placement names (no std::source_location record falls in the destructor, so its line
# is not evidence). Its body is the two std::unique_ptr<IThreadImpl> members (+12 m_impl, +16 m_implEspera) reset in
# reverse order; IThreadImpl (api/ipc/ithreadimpl.h) has a virtual destructor, slot 1 of its vtable. The constructor
# (3598) and Start (1684) are not reconstructed yet. Paths relative to src/uenux2.
CTHREAD_SRCS := src/api/ipc/cthread.cpp
UENUX2_SRCS += $(CTHREAD_SRCS)
# the roots that keep the destructor linked (see the file header)
$(ROOT)/$(APP)/18-uenux2-cthread-roots.o: $(ROOT)/src/units/18-uenux2-cthread-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/18-uenux2-cthread-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/18-uenux2-cthread-roots.o
