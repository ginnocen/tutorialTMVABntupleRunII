#!/bin/bash

DOTMVA=1
DOMAKEVAR=1
DOMERGE=1
DOREADXML=1
#
OUTPUTDIR="/data/wangj/TutorialsSamples/test"
if [ ! -d $OUTPUTDIR ]; then
    mkdir $OUTPUTDIR
fi

#
# if working on several ptbins, BDT.C need to be changed
PTBIN=(7 10)
RAA=(0.49)
COLSYST=('PbPb')
isPbPb=(1)

MVA=('BDT')
#MVA=('CutsSA')

nPT=$((${#PTBIN[@]}-1))
nMVA=${#MVA[@]}
nCOL=${#COLSYST[@]}

#
FOLDERS=("myTMVA/weights" "myTMVA/ROOT" "mva/MVAfiles" "readxml/plots")
for i in ${FOLDERS[@]}
do
    if [ ! -d $i ]; then
        mkdir $i
    fi
done
#

# TMVAClassification.C #
if [ $DOTMVA -eq 1 ]; then
    j=0
    while ((j<$nCOL))
    do
        i=0
        while ((i<$nPT))
        do
	    cd myTMVA/
	    echo "-- Processing pT bin: ${PTBIN[i]} - ${PTBIN[i+1]} GeV/c"
	    echo
	    root -l -b -q 'TMVAClassification.C+('${isPbPb[j]}','${PTBIN[i]}','${PTBIN[i+1]}')'
            
	    k=0
	    while ((k<$nMVA))
	    do
	        mv weights/TMVAClassification_${MVA[k]}.weights.xml weights/TMVA_${MVA[k]}_${COLSYST[j]}_${PTBIN[i]}_${PTBIN[i+1]}.weights.xml
	        mv weights/TMVAClassification_${MVA[k]}.class.C weights/TMVA_${MVA[k]}_${COLSYST[j]}_${PTBIN[i]}_${PTBIN[i+1]}.class.C
	        k=$(($k+1))
	    done
	    cd ..
	    i=$(($i+1))    
        done
        j=$(($j+1))
    done    
fi

# Tree Variables #
if [ $DOMAKEVAR -eq 1 ]; then
    k=0
    while ((k<$nMVA))
    do
        j=0
        if [ "${MVA[k]}"x = "BDT"x ]; then
            while ((j<$nCOL))
            do
                i=0
	        while ((i<$nPT))
	        do
		    cd mva/
		    root -l -b -q 'classmaker.C("'../myTMVA/weights/TMVA_${MVA[k]}_${COLSYST[j]}_${PTBIN[i]}_${PTBIN[i+1]}.class.C'")'
		    cd macros/
		    root -l -b -q ''${MVA[k]}'.C('${isPbPb[j]}','${PTBIN[i]}','${PTBIN[i+1]}',"'${MVA[k]}'")' 
		    cd ../..
	            i=$(($i+1))
	        done
                j=$(($j+1))
            done
        fi
        k=$(($k+1))
    done
fi

# mergeBDT.C and readxml.cc #
inputMCs=("/data/wangj/TutorialsSamples/tmvaBntupleRunII/Bntuple20160816_Bpt7svpv5p5Bpt10svpv3p5_BfinderMC_pp_Pythia8_BuToJpsiK_Bpt0_Pthat5_TuneCUEP8M1_20160816_bPt0jpsiPt0tkPt0p5_Bp.root"
    "/data/wangj/TutorialsSamples/tmvaBntupleRunII/Bntuple20160816_Bpt7svpv5p5Bpt10svpv3p5_BfinderMC_PbPb_Pythia8_BuToJpsiK_Bpt0_Pthat5_TuneCUEP8M1_20160816_bPt5jpsiPt0tkPt0p8_Bp.root")
inputDatas=("/data/wangj/TutorialsSamples/tmvaBntupleRunII/Bntuple20160816_Bpt7svpv5p5Bpt10svpv3p5_BfinderData_pp_20160816_bPt0jpsiPt0tkPt0p5_Bp.root"
    "/data/wangj/TutorialsSamples/tmvaBntupleRunII/Bntuple20160816_Bpt7svpv5p5Bpt10svpv3p5_BfinderData_PbPb_20160816_bPt5jpsiPt0tkPt0p8_Bp.root")
k=0
while ((k<$nMVA))
do
    if [ "${MVA[k]}"x = "BDT"x ]; then
        j=0
        while ((j<$nCOL))
        do
            i=0
            # if working on several ptbins, BDT.C need to be changed
            if [ $nPT -eq 1 ]; then
                while ((i<$nPT))
                do
                    outputMC="${OUTPUTDIR}/ntB_${COLSYST[j]}_MC_${PTBIN[i]}_${PTBIN[i+1]}_BDT_Pthat5.root"
                    outputData="${OUTPUTDIR}/ntB_${COLSYST[j]}_PbPb_Data_${PTBIN[i]}_${PTBIN[i+1]}_BDT_Pthat5.root"
                    inputMC=${inputMCs[${isPbPb[j]}]}
                    inputData=${inputDatas[${isPbPb[j]}]}
                    if [ $DOMERGE -eq 1 ]; then
                        echo
                        echo "  Processing mergeBDT.sh - ${isPbPb[j]}"
                        echo
                        if [ -f $outputMC ]; then
                            echo "  Error: Targed merged file exists: $outputMC"
                        else
                            hadd $outputMC $inputMC mva/MVAfiles/BDT_${COLSYST[j]}_${PTBIN[i]}_${PTBIN[i+1]}_MC.root
                        fi
                        if [ -f $outputData ]; then
                            echo "  Error: Targed merged file exists: $outputData"
                        else
                            hadd $outputData $inputData mva/MVAfiles/BDT_${COLSYST[j]}_${PTBIN[i]}_${PTBIN[i+1]}_DATA.root
                        fi
                    fi
                    if [ $DOREADXML -eq 1 ]; then
                        if [ ! -f $outputMC ]; then
                            echo " Error: Merged BDT trees before readxml.cc : MC"
                            echo
                        elif [ ! -f $outputData ]; then
                            echo " Error: Merged BDT trees before readxml.cc : Data"
                            echo
                        else
                            cd readxml/
                            root -b -q "readxml_BDT.cc+("\"$outputMC\"","\"$outputData\"","${isPbPb[j]}","\"${MVA[k]}\"","${PTBIN[i]}","${PTBIN[i+1]}","${RAA[i]}")"
                            cd ..
                        fi
                    fi
                    i=$(($i+1))
                done
            else
                echo "  Error: If working on several ptbins, BDT.C need to be changed"
                echo
            fi
            j=$(($j+1))
        done
    else
        j=0
        while ((j<$nCOL))
        do
            i=0
            while ((i<$nPT))
            do
                if [ $DOREADXML -eq 1 ]; then
                    cd readxml/
                    root -b -q "readxml.cc+("${isPbPb[j]}","\"${MVA[k]}\"","${PTBIN[i]}","${PTBIN[i+1]}","${RAA[i]}")"
                    cd ..
                fi
                i=$(($i+1))
            done
            j=$(($j+1))
        done
    fi
    k=$(($k+1))
done

