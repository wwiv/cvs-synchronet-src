CC	= gcc
CXX	= g++
SRC_ROOT	:=	../../..
include ${SRC_ROOT}/build/Common.gmake

ODOORS_SRC	:= $(SRC_ROOT)/odoors
CFLAGS += $(CIOLIB-MT_CFLAGS) $(XPDEV-MT_CFLAGS)
CFLAGS += -I$(ODOORS_SRC)
LDFLAGS += $(CIOLIB-MT_LDFLAGS) $(XPDEV-MT_LDFLAGS) -L${ODOORS_SRC}/libs-`uname`
CPPFLAGS += $(CFLAGS)

$(NY2008) : $(OBJS)
	$(CXX) $(CPPFLAGS) ${LDFLAGS} $(OBJS) -lODoors -o $(NY2008) $(CIOLIB-MT_LIBS) $(XPDEV-MT_LIBS)

$(AEDIT) : $(AEDIT_OBJS)
	$(CXX) $(CPPFLAGS) ${LDFLAGS} $(AEDIT_OBJS) -o $(AEDIT)

$(MEDIT) : $(MEDIT_OBJS)
	$(CXX) $(CPPFLAGS) ${LDFLAGS} $(MEDIT_OBJS) -o $(MEDIT)

$(NYIBBS) : $(NYIBBS_OBJS)
	$(CXX) $(CPPFLAGS) ${LDFLAGS} $(NYIBBS_OBJS) -o $(NYIBBS) $(XPDEV-MT_LIBS)

$(NYEDIT) : $(NYEDIT_OBJS)
	$(CXX) $(CPPFLAGS) ${LDFLAGS} $(NYEDIT_OBJS) -lODoors -o $(NYEDIT) $(CIOLIB-MT_LIBS) $(XPDEV-MT_LIBS)

