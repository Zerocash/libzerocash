OPTFLAGS = -march=native -mtune=native -O2
CXXFLAGS += -g -Wall -Wextra -Werror -Wfatal-errors -Wno-unused-parameter -std=c++11 -fPIC -Wno-unused-variable
LDFLAGS += -flto

ifeq ($(CURVE),)
	CURVE = ALT_BN128
endif


DEPSRC=depsrc
DEPINST=depinst

LIBZEROCASH=libzerocash
UTILS=$(LIBZEROCASH)/utils
TESTUTILS=tests
LDLIBS += -L $(DEPINST)/lib -Wl,-rpath $(DEPINST)/lib -L . -lsnark -lgmpxx -lgmp
LDLIBS += -lboost_system -lcrypto -lcryptopp -lz -ldl
CXXFLAGS += -I $(DEPINST)/include -I $(DEPINST)/include/libsnark -I . -DUSE_ASM -DCURVE_$(CURVE)

LIBPATH = /usr/local/lib

SRCS= \
	$(UTILS)/sha256.cpp \
	$(UTILS)/util.cpp \
	$(LIBZEROCASH)/Node.cpp \
	$(LIBZEROCASH)/IncrementalMerkleTree.cpp \
	$(LIBZEROCASH)/MerkleTree.cpp \
	$(LIBZEROCASH)/Address.cpp \
	$(LIBZEROCASH)/CoinCommitment.cpp \
	$(LIBZEROCASH)/Coin.cpp \
	$(LIBZEROCASH)/MintTransaction.cpp \
	$(LIBZEROCASH)/PourTransaction.cpp \
	$(LIBZEROCASH)/ZerocashParams.cpp \
	$(TESTUTILS)/timer.cpp

EXECUTABLES= \
	zerocash_pour_ppzksnark/tests/test_zerocash_pour_ppzksnark \
	zerocash_pour_ppzksnark/profiling/profile_zerocash_pour_gadget \
	tests/zerocashTest \
	tests/merkleTest \
	libzerocash/GenerateParamsForFiles

OBJS=$(patsubst %.cpp,%.o,$(SRCS))

DOCS=README.html

ifeq ($(MINDEPS),1)
	CXXFLAGS += -DMINDEPS
else
	LDLIBS += -lboost_program_options
	LDLIBS += -lprocps
endif

ifeq ($(LOWMEM),1)
	CXXFLAGS += -DLOWMEM
endif

ifeq ($(STATIC),1)
	CXXFLAGS += -static -DSTATIC
endif

ifeq ($(PROFILE_CURVE),1)
	CXXFLAGS += -static -DPROFILE_CURVE
endif

ifeq ($(MULTICORE),1)
	# When running ./get-libsnark to prepare for this build, use:
	# $ LIBSNARK_FLAGS='MULTICORE=1 STATIC=1' ./get-libsnark.
	# If you're missing some static libraries, it may help to also add
	# $ NO_PROCPS=1  ...  ./get-libsnark
	# and pass MINDEPS=1 to this makefile
	# and run ./get-cryptopp to build the static cryptopp library.
	CXXFLAGS += -static -fopenmp -DMULTICORE
endif

all: $(EXECUTABLES) libzerocash.a

cppdebug: CXXFLAGS += -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
cppdebug: debug

debug: CXXFLAGS += -DDEBUG -g -ggdb3
debug: all

noasserts: CXXFLAGS += -DNDEBUG -Wno-unused-variable -Wno-unused-but-set-variable
noasserts: all

# In order to detect changes to #include dependencies. -MMD below generates a .d file for .cpp file. Include the .d file.
-include $(SRCS:.cpp=.d)

$(OBJS) ${patsubst %,%.o,${EXECUTABLES}}: %.o: %.cpp
	$(CXX) -o $@ $< -c -MMD $(CXXFLAGS)

$(EXECUTABLES): %: %.o $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

libzerocash: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	$(CXX) -shared -o "libzerocash.so" $(OBJS) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)
	@echo 'Finished building target: $@'
	@echo 'Copying libzerocash.so'
	sudo cp libzerocash.so $(LIBPATH)/libzerocash.so
	sudo ldconfig
	@echo 'Finished copying libzerocash.so'
	@echo ' '

libzerocash.a: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	$(AR) rcvs $@ $(OBJS)
	@echo 'Finished building target: $@'
	#@echo 'Copying libzerocash.a'
	#sudo cp libzerocash.a $(LIBPATH)/libzerocash.a
	#sudo ldconfig
	#@echo 'Finished copying libzerocash.a'
	@echo ' '

test_library: %: tests/zerocashTest.o $(OBJS)
	$(CXX) -o tests/$@ $^ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -lzerocash

banktest_library: %: bankTest.o $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -lzerocash

merkletest_library: %: merkleTest.o $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -lzerocash

$(DOCS): %.html: %.md
	markdown_py -f $@ $^ -x toc -x extra --noisy
#	TODO: Would be nice to enable "-x smartypants" but Ubuntu 12.04 doesn't support that.
#	TODO: switch to redcarpet, to produce same output as GitHub's processing of README.md. But what about TOC?

doc: $(DOCS)

.PHONY: clean doc all

clean:
	$(RM) \
		$(OBJS) \
		$(EXECUTABLES) \
		${patsubst %,%.o,${EXECUTABLES}} \
		${patsubst %.cpp,%.d,${SRCS}} \
		libzerocash.a \
		tests/test_library
