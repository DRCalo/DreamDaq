BINDIR	:= $(HOME)/bin

APPS	:= cleanDAQ ipcclean ise impact cleanup.sh dreamfunct.sh dreamProc.sh sshDreamProc.sh takeCamScreenshots.sh doOfflineHisto auto_make_ntuples.sh do_make_ntuples.sh backupDataFile backup_to_ttu.sh getinfo tolower show_monitoring_plots.cxx tower_map_gen.cxx loadFPGA.cmd loadFPGA.sh desyfunct.sh desyProc.sh run4ever.sh runSPS2021.sh stopSPS2021.sh checkSPS2021.sh hkillSPS2021.sh runSPS2021nos.sh stopSPS2021nos.sh checkSPS2021nos.sh hkillSPS2021nos.sh testNumber.sh fixLostTriggers.sh runSPS2023nos.sh stopSPS2023nos.sh checkSPS2023nos.sh hkillSPS2023nos.sh runSPS2024nos.sh stopSPS2024nos.sh checkSPS2024nos.sh hkillSPS2024nos.sh runDEBUG.sh stopDEBUG.sh

install: tolower
	@mkdir -p $(BINDIR)
	/bin/cp -p $(APPS) $(BINDIR)/

tolower: tolower.cpp
	g++ -o $@ $^

