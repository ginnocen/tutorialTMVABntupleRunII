
<h1> tutorialTMVABntupleRunII </h1>
<p> tutorialTMVABntupleRunII is an example to run cut optimization for B+ analysis </p>

<h2> How to run </h2>
<ul>
<li> Set output directory in <code>doTMVA.sh</code> with a path you have right to write.<br />
<pre>
OUTPUTDIR="/data/wangj/TutorialsSamples/tmvaBntupleRunII"
</pre>
</li>
<li> Run the macros <br />
<pre> 
source doTMVA.sh
</pre> 
<p> without any modification. </p>
</li>
</ul>
<p> The default setting is cut optimization with BDT method for PbPb samples in pT range 7~10 GeV/c. </p>

<h2> Brief explanation to each macro </h2>

<h3> myTMVA/ </h3>
<ol>
<li> <code>TMVAClassification.C</code> does machine learning to classify signal and background. </li>
<li> More details see the slides of previous tutorial <a href="https://github.com/boundino/tutorialTMVA/blob/master/20160608_tutorialTMVA.pdf">tutorialTMVA</a>. </li>
</ol>

<h3> mva/ </h3>
<code> macros/BDT.C </code> builds a tree with branch of BDT value for each candidate.

<h3> Merging BDT tree in the samples </h3>
<p> The lines are at <code>doTMVA.sh</code> <a href="https://github.com/boundino/tutorialTMVABntupleRunII/blob/master/doTMVA.sh#L103-L121">L103-L121</a>. </p>

<h3> readxml/ </h3>
<ol>
<li> <code>readxml_BDT.cc</code> is for BDT method. It produces <a href="https://github.com/boundino/tutorialTMVABntupleRunII/blob/master/plotsREADME/BDT_PbPb_pT_7_10_disEffSB.pdf">BDT distribution</a>, <a href="https://github.com/boundino/tutorialTMVABntupleRunII/blob/master/plotsREADME/BDT_PbPb_pT_7_10_EffvsBDT.pdf">Efficiency vs. BDT cut value</a>, <a href="https://github.com/boundino/tutorialTMVABntupleRunII/blob/master/plotsREADME/BDT_PbPb_pT_7_10_Significance.pdf">S/sqrt(S+B) vs. BDT cut value</a>, etc. 
<img src="https://github.com/boundino/tutorialTMVABntupleRunII/blob/master/plotsREADME/BDT_PbPb_pT_7_10_Significance.pdf" alt="Significance" style="width:304px;height:228px;">
</li>
<li> Print out:
<img src="https://github.com/boundino/tutorialTMVABntupleRunII/blob/master/plotsREADME/tmvaPrintout.png" alt="Printout" style="width:304px;height:228px">
</li>
</ol>
