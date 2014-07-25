OS := $(shell uname -o)
ifeq (${OS}, Cygwin)
EXE := .exe
endif

sight_H := ../*.h ../*/*.h ../widgets/*/*.h
TESTERS = 8.Modules${EXE} 9.CompModules.merged${EXE} 9.CompModules.single${EXE} 5.Tracing${EXE} 0.Demo${EXE} 11.ExternTraceProcess${EXE} 10.SpringModules${EXE} \
          1.StructuredFormatting${EXE} 2.ConditionalFormatting${EXE} 3.Navigation${EXE} \
          4.AttributeAnnotationFiltering${EXE} 6.PerfAnalysis${EXE} \
          7.Merging${EXE} 

all: ${TESTERS}

run: ${TESTERS}
	# 0.Demo
	export SIGHT_FILE_OUT=1; ./0.Demo${EXE} 5; ./0.Demo${EXE} 6; ./0.Demo${EXE} 7
	export SIGHT_FILE_OUT=1; rm -rf dbg.0.Demo; ../hier_merge dbg.0.Demo common dbg.0.Demo.maxDepth_5/structure dbg.0.Demo.maxDepth_6/structure dbg.0.Demo.maxDepth_7/structure
	#rm -rf dbg.0.Demo.*;
	../slayout${EXE} dbg.0.Demo/structure;

0.Demo${EXE}: 0.Demo.C ../libsight_structure.so ${sight_H}
	${CCC} ${SIGHT_CFLAGS} -DROOT_PATH="\"${ROOT_PATH}\"" 0.Demo.C -I.. -I../widgets -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 0.Demo${EXE}

1.StructuredFormatting${EXE}: 1.StructuredFormatting.C ../libsight_structure.so ${sight_H}
	${CCC} ${SIGHT_CFLAGS} 1.StructuredFormatting.C -I.. -I../widgets -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 1.StructuredFormatting${EXE}

2.ConditionalFormatting${EXE}: 2.ConditionalFormatting.C ../libsight_structure.so ${sight_H}
	${CCC} ${SIGHT_CFLAGS} 2.ConditionalFormatting.C -I.. -I../widgets -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 2.ConditionalFormatting${EXE}

3.Navigation${EXE}: 3.Navigation.C ../libsight_structure.so ${sight_H}
	${CCC} ${SIGHT_CFLAGS} 3.Navigation.C -I.. -I../widgets -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 3.Navigation${EXE}

4.AttributeAnnotationFiltering${EXE}: 4.AttributeAnnotationFiltering.C ../libsight_structure.so ${sight_H}
	${CCC} ${SIGHT_CFLAGS} 4.AttributeAnnotationFiltering.C -I.. -I../widgets -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 4.AttributeAnnotationFiltering${EXE}

5.Tracing${EXE}: 5.Tracing.C ../libsight_structure.so ${sight_H}
	${CCC} ${SIGHT_CFLAGS} 5.Tracing.C -I.. -I../widgets -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 5.Tracing${EXE}

6.PerfAnalysis${EXE}: 6.PerfAnalysis.C ../libsight_structure.so ${sight_H}
	${CCC} ${SIGHT_CFLAGS} 6.PerfAnalysis.C -I.. -I../widgets -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 6.PerfAnalysis${EXE}

7.Merging${EXE}: 7.Merging.C ../libsight_structure.so ${sight_H}
	${CCC} ${SIGHT_CFLAGS} 7.Merging.C -I.. -I../widgets -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 7.Merging${EXE}

8.Modules${EXE}: 8.Modules.C ../libsight_structure.so ${sight_H}
	${CCC} ${SIGHT_CFLAGS} 8.Modules.C -I.. -I../widgets -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 8.Modules${EXE}

9.CompModules.single${EXE}: 9.CompModules.C ../libsight_structure.so ${sight_H}
	${CCC} ${SIGHT_CFLAGS} 9.CompModules.C -DTRACE -I.. -I../widgets -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 9.CompModules.single${EXE}

9.CompModules.merged${EXE}: 9.CompModules.C ../libsight_structure.so ${sight_H}
	${CCC} ${SIGHT_CFLAGS} 9.CompModules.C -I.. -I../widgets -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 9.CompModules.merged${EXE}

10.SpringModules${EXE}: 10.SpringModules.C ../libsight_structure.so ${sight_H}
	${CCC} ${SIGHT_CFLAGS} 10.SpringModules.C -I.. -I../widgets -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 10.SpringModules${EXE}

11.ExternTraceProcess${EXE}: 11.ExternTraceProcess.C ../libsight_structure.so ${sight_H}
	${CCC} -g 11.ExternTraceProcess.windowing.C -I.. -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 11.ExternTraceProcess.windowing${EXE}
	${CCC} -g ${SIGHT_CFLAGS} 11.ExternTraceProcess.C -I.. -I../widgets -L.. -lsight_structure ${SIGHT_LINKFLAGS} -o 11.ExternTraceProcess${EXE}

clean:
	rm -rf ${TESTERS} dbg.* *.core
