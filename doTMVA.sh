#!/bin/bash

DOTMVA=1
DOMAKEVAR=1
DOMERGE=1
DOREADXML=1
# modify OUTPUTDIR to a path you can write in #
OUTPUTDIR="/data/wangj/TutorialsSamples/tmvaBntupleRunII/outputs/"
if [ ! -d $OUTPUTDIR ]
then
    mkdir -p $OUTPUTDIR
fi

#
PTMIN=(7 10)
PTMAX=(10 15)
RAA=(0.49 0.45)
#
MVA=('BDT' 'CutsSA')
#
COLSYST=('PbPb')
LUMI=(15.17)
INPUTSNAME=('/data/wangj/TutorialsSamples/tmvaBntupleRunII/Bntuple20160816_Bpt7svpv5p5Bpt10svpv3p5_BfinderMC_PbPb_Pythia8_BuToJpsiK_Bpt0_Pthat5_TuneCUEP8M1_20160816_bPt5jpsiPt0tkPt0p8_Bp.root')
INPUTBNAME=('/data/wangj/TutorialsSamples/tmvaBntupleRunII/Bntuple20160816_Bpt7svpv5p5Bpt10svpv3p5_BfinderData_PbPb_20160816_bPt5jpsiPt0tkPt0p8_Bp.root')
OUTPUTSNAME=('Bntuple20160816_Bpt7svpv5p5Bpt10svpv3p5_BfinderMC_PbPb_Pythia8_BuToJpsiK_Bpt0_Pthat5_TuneCUEP8M1_20160816_bPt5jpsiPt0tkPt0p8_Bp')
OUTPUTBNAME=('Bntuple20160816_Bpt7svpv5p5Bpt10svpv3p5_BfinderData_PbPb_20160816_bPt5jpsiPt0tkPt0p8_Bp')
INPUTMCNAME=("${INPUTSNAME[0]}")
INPUTDANAME=("${INPUTBNAME[0]}")
CUT=('pclusterCompatibilityFilter&&pprimaryVertexFilter&&phfCoincFilter3&abs(PVz)<15&&TMath::Abs(By)<2.4&&TMath::Abs(Bmumumass-3.096916)<0.15&&Bmass>5&&Bmass<6&&((abs(Bmu1eta)<1.2&&Bmu1pt>3.5)||(abs(Bmu1eta)>1.2&&abs(Bmu1eta)<2.1&&Bmu1pt>(5.77-1.8*abs(Bmu1eta)))||(abs(Bmu1eta)>2.1&&abs(Bmu1eta)<2.4&&Bmu1pt>1.8))&&((abs(Bmu2eta)<1.2&&Bmu2pt>3.5)||(abs(Bmu2eta)>1.2&&abs(Bmu2eta)<2.1&&Bmu2pt>(5.77-1.8*abs(Bmu2eta)))||(abs(Bmu2eta)>2.1&&abs(Bmu2eta)<2.4&&Bmu2pt>1.8))&&Bmu1TMOneStationTight&&Bmu2TMOneStationTight&&Bmu1InPixelLayer>0&&(Bmu1InPixelLayer+Bmu1InStripLayer)>5&&Bmu2InPixelLayer>0&&(Bmu2InPixelLayer+Bmu2InStripLayer)>5&&Bmu1dxyPV<0.3&&Bmu2dxyPV<0.3&&Bmu1dzPV<20&&Bmu2dzPV<20&&Bmu1isGlobalMuon&&Bmu2isGlobalMuon&&Bmu1TrgMatchFilterE>0&&Bmu2TrgMatchFilterE>0&&Btrk1highPurity&&abs(Btrk1Eta)<2.4&&Btrk1Pt>0.8&&Bchi2cl>0.005&&((Bpt<10&&(BsvpvDistance/BsvpvDisErr)>5.5)||(Bpt>10&&(BsvpvDistance/BsvpvDisErr)>3.5))')
MYCUTS=("${CUT[0]}&&Bgen==23333")
MYCUTB=("${CUT[0]}&&TMath::Abs(Bmass-5.279)>0.2&&TMath::Abs(Bmass-5.279)<0.3")
MYCUTG=("abs(Gy)<2.4&&abs(GpdgId)==521&&GisSignal==1");

# do not touch the lines below if you don't know what they mean #

##
nPT=${#PTMIN[@]}
nMVA=${#MVA[@]}
nCOL=${#COLSYST[@]}

#
NC='\033[0m'

#
function float_to_string()
{
    if [[ $# -ne 1 ]]
    then
        echo -e "\033[1;31merror:${NC} invalid argument number - float_to_string()"
        exit 1
    fi
    part1=`echo $1 | awk -F "." '{print $1}'`
    part2=`echo $1 | awk -F "." '{print $2}'`
    rt_float_to_string=${part1:-0}p${part2:-0}
    echo $rt_float_to_string
}

#
FOLDERS=("myTMVA/weights" "myTMVA/ROOT" "mva/MVAfiles" "readxml/plots" "readxml/results")
for i in ${FOLDERS[@]}
do
    if [ ! -d $i ]
    then
        mkdir -p $i
    fi
done
#

# TMVAClassification.C #
if [ $DOTMVA -eq 1 ]
then
    cd myTMVA/
    g++ TMVAClassification.C $(root-config --cflags --libs) -l TMVA -g -o TMVAClassification.exe
    j=0
    while ((j<$nCOL))
    do
        i=0
        while ((i<$nPT))
        do
            tPT=pt_$(float_to_string ${PTMIN[i]})_$(float_to_string ${PTMAX[i]})
            echo -e "-- Processing \033[1;33mTMVAClassification.C${NC}, \033[1;32m${COLSYST[j]}${NC}, \033[1;32m${PTMIN[i]} - ${PTMAX[i]} GeV/c${NC}"
            set -x
            ./TMVAClassification.exe "${INPUTSNAME[j]}" "${INPUTBNAME[j]}" "${COLSYST[j]}" "${MYCUTS[j]}" "${MYCUTB[j]}" ${PTMIN[i]} ${PTMAX[i]}
	    k=0
	    while ((k<$nMVA))
	    do
                TEND=TMVA_${MVA[k]}_${COLSYST[j]}_${tPT}
	        mv weights/TMVAClassification_${MVA[k]}.weights.xml weights/${TEND}.weights.xml
	        mv weights/TMVAClassification_${MVA[k]}.class.C weights/${TEND}.class.C
                mv ROOT/tmva.root ROOT/${TEND}.root
	        k=$(($k+1))
	    done
            set +x
	    i=$(($i+1))
            echo
        done
        j=$(($j+1))
    done    
    rm TMVAClassification.exe
    cd ..
fi

# Tree Variables #
if [ $DOMAKEVAR -eq 1 ]
then
    k=0
    while ((k<$nMVA))
    do
        j=0
        if [[ ${MVA[k]} != Cuts* ]]
        then            
            while ((j<$nCOL))
            do
                i=0
	        while ((i<$nPT))
	        do
                    tPT=pt_$(float_to_string ${PTMIN[i]})_$(float_to_string ${PTMAX[i]})
                    TEND=TMVA_${MVA[k]}_${COLSYST[j]}_${tPT}
                    echo -e "-- Processing \033[1;33m${MVA[k]}.C${NC}, \033[1;32m${COLSYST[j]}${NC}, \033[1;32m${PTMIN[i]} - ${PTMAX[i]} GeV/c${NC}"
                    if [ -f "myTMVA/weights/${TEND}.class.C" ]
                    then
		        cd mva/
                        g++ classmaker.C $(root-config --cflags --libs) -g -o classmaker.exe
                        set -x
                        ./classmaker.exe "../myTMVA/weights/${TEND}.class.C"
                        set +x
                        rm classmaker.exe
		        cd macros/
                        g++ ${MVA[k]}.C $(root-config --cflags --libs) -l TMVA -g -o ${MVA[k]}.exe
                        set -x
                        ./${MVA[k]}.exe "${INPUTSNAME[j]}" "../MVAfiles/mvafiles_${OUTPUTSNAME[j]}_${TEND}" ${PTMIN[i]} ${PTMAX[i]} "${MVA[k]}"
                        ./${MVA[k]}.exe "${INPUTBNAME[j]}" "../MVAfiles/mvafiles_${OUTPUTBNAME[j]}_${TEND}" ${PTMIN[i]} ${PTMAX[i]} "${MVA[k]}"
                        set +x
                        rm ${MVA[k]}.exe
		        cd ..
                        rm header.h
                        cd ..
                    else
                        echo -e "\033[1;31merror:${NC} no weight file: myTMVA/weights/${TEND}.class.C"
                    fi
                    echo
	            i=$(($i+1))
	        done
                j=$(($j+1))
            done
        fi
        k=$(($k+1))
    done
fi

# Merge mva trees #
if [ $DOMERGE -eq 1 ]
then
    k=0
    while ((k<$nMVA))
    do
        if [[ "${MVA[k]}" != Cuts* ]]
        then
            j=0
            while ((j<$nCOL))
            do
                OUTPUTS="${OUTPUTDIR}/${OUTPUTSNAME[j]}_TMVA_${MVA[k]}_${COLSYST[j]}.root"
                echo -e "-- Processing \033[1;33mmerge${NC}, \033[1;32m${COLSYST[j]}${NC} MC, \033[1;32m${OUTPUTS}${NC}"
                if [ -f $OUTPUTS ]
                then
                    echo -e "\033[1;31merror:${NC} target merged file exists: $OUTPUTS"
                else
                    HADDINPUTS=""
                    i=0
                    while ((i<$nPT))
                    do
                        tPT=pt_$(float_to_string ${PTMIN[i]})_$(float_to_string ${PTMAX[i]})
                        TEND=TMVA_${MVA[k]}_${COLSYST[j]}_${tPT}
                        if [ -f "mva/MVAfiles/mvafiles_${OUTPUTSNAME[j]}_${TEND}.root" ]
                        then
                            HADDINPUTS="${HADDINPUTS} mva/MVAfiles/mvafiles_${OUTPUTSNAME[j]}_${TEND}.root"
                        else
                            echo -e "\033[1;31merror:${NC} no input file: mva/MVAfiles/mvafiles_${OUTPUTSNAME[j]}_${TEND}.root"
                        fi
                        i=$(($i+1))
                    done
                    if [[ $HADDINPUTS != "" ]]
                    then
                        set -x
                        hadd $OUTPUTS ${INPUTSNAME[j]} $HADDINPUTS
                        set +x
                    else
                        echo -e "\033[1;31merror:${NC} no input files for $OUTPUTS"
                    fi
                fi
                echo
                OUTPUTB="${OUTPUTDIR}/${OUTPUTBNAME[j]}_TMVA_${MVA[k]}_${COLSYST[j]}.root"
                echo -e "-- Processing \033[1;33mmerge${NC}, \033[1;32m${COLSYST[j]}${NC} data, \033[1;32m${OUTPUTB}${NC}"
                if [ -f $OUTPUTB ]
                then
                    echo -e "\033[1;31merror:${NC} target merged file exists: $OUTPUTB"
                else
                    HADDINPUTB=""
                    i=0
                    while ((i<$nPT))
                    do
                        tPT=pt_$(float_to_string ${PTMIN[i]})_$(float_to_string ${PTMAX[i]})
                        TEND=TMVA_${MVA[k]}_${COLSYST[j]}_${tPT}
                        if [ -f "mva/MVAfiles/mvafiles_${OUTPUTBNAME[j]}_${TEND}.root" ]
                        then
                            HADDINPUTB="${HADDINPUTB} mva/MVAfiles/mvafiles_${OUTPUTBNAME[j]}_${TEND}.root"
                        else
                            echo -e "\033[1;31merror:${NC} no input file: mva/MVAfiles/mvafiles_${OUTPUTBNAME[j]}_${TEND}.root"
                        fi
                        i=$(($i+1))
                    done
                    if [[ $HADDINPUTB != "" ]]
                    then
                        set -x
                        hadd $OUTPUTB ${INPUTBNAME[j]} $HADDINPUTB
                        set +x
                    else
                        echo -e "\033[1;31merror:${NC} no input files for $OUTPUTB"
                    fi
                fi
                echo
                j=$(($j+1))
            done
        fi
        k=$(($k+1))
    done
fi

# readxml*.cc #
if [ $DOREADXML -eq 1 ]
then
    cd readxml/
    g++ readxml.cc $(root-config --cflags --libs) -l TMVA -l XMLIO -g -o readxml.exe
    g++ readxml_Cuts.cc $(root-config --cflags --libs) -l TMVA -l XMLIO -g -o readxml_Cuts.exe
    j=0
    while ((j<$nCOL))
    do
        i=0
        while ((i<$nPT))
        do
            k=0
            while ((k<$nMVA))
            do
                tPT=pt_$(float_to_string ${PTMIN[i]})_$(float_to_string ${PTMAX[i]})
                TEND=TMVA_${MVA[k]}_${COLSYST[j]}_${tPT}
                echo -e "-- Processing \033[1;33mreadxml*.cc${NC}, \033[1;32m${COLSYST[j]}${NC}, \033[1;32m${MVA[k]}${NC}, \033[1;32m${PTMIN[i]} - ${PTMAX[i]} GeV/c${NC}"
                if [[ "${MVA[k]}" != Cuts* ]]
                then 
                    OUTPUTS="${OUTPUTDIR}/${OUTPUTSNAME[j]}_TMVA_${MVA[k]}_${COLSYST[j]}.root"
                    OUTPUTB="${OUTPUTDIR}/${OUTPUTBNAME[j]}_TMVA_${MVA[k]}_${COLSYST[j]}.root"
                    if [ ! -f $OUTPUTS ]
                    then
                        echo -e "\033[1;31merror:${NC} no merged input file: $OUTPUTS"
                        echo
                    elif [ ! -f $OUTPUTB ]
                    then
                        echo -e "\033[1;31merror:${NC} no merged input file: $OUTPUTB"
                        echo
                    elif [ ! -f "../myTMVA/weights/${TEND}.weights.xml" ]
                    then
                        echo -e "\033[1;31merror:${NC} no weight file: myTMVA/weights/${TEND}.weights.xml"
                    else
                        set -x
                        ./readxml.exe "$OUTPUTS" "$OUTPUTB" "$TEND" "../myTMVA/weights/${TEND}.weights.xml" "${COLSYST[j]}" "${MYCUTS[j]}" "${MYCUTB[j]}" "${MYCUTG[j]}" "1" "${MVA[k]}" ${PTMIN[i]} ${PTMAX[i]} ${LUMI[j]} ${RAA[j]}
                        set +x
                    fi
                else
                    if [ ! -f "../myTMVA/weights/${TEND}.weights.xml" ]
                    then
                        echo -e "\033[1;31merror:${NC} no weight file: myTMVA/weights/${TEND}.weights.xml"
                    else
                        set -x
                        ./readxml_Cuts.exe "${INPUTSNAME[j]}" "${INPUTBNAME[j]}" "$TEND" "../myTMVA/weights/${TEND}.weights.xml" "${COLSYST[j]}" "${MYCUTS[j]}" "${MYCUTB[j]}" "${MYCUTG[j]}" "1" "${MVA[k]}" ${PTMIN[i]} ${PTMAX[i]} ${LUMI[j]} ${RAA[j]}
                        set +x
                    fi
                fi
                echo
                k=$(($k+1))
            done
            i=$(($i+1))
        done
        j=$(($j+1))
    done
    rm readxml_Cuts.exe
    rm readxml.exe
    cd ..
fi
