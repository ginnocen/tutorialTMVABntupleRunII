
<h1> tutorialTMVABntupleRunII </h1>
<p> tutorialTMVABntupleRunII is an example to run cut optimization for B+ analysis in PbPb collisions at 5.02 TeV</p>

<h2> How to run </h2>
<ol>
<li> Set output directory in <code>doTMVA.sh</code> <a href="https://github.com/boundino/tutorialTMVABntupleRunII/blob/master/doTMVA.sh#L8">L8</a> with a path you have right to write.<br />
<pre>
OUTPUTDIR="/data/wangj/TutorialsSamples/tmvaBntupleRunII/outputs/"
</pre>
</li>
<li> Run the macros<br />
<pre>
source doTMVA.sh
</pre>
<p> without any modification. </p>
</li>
</ol>
<p> The default setting is cut optimization with BDT and CutsSA methods in pT range 7~10 and 10~15 GeV/c. </p>

<h2> Brief explanation to each macro </h2>

<h3> myTMVA/ </h3>
<ul>
<li> <code>TMVAClassification.C</code> does machine learning to classify signal and background. </li>
<li> More details see the slides of previous tutorial <a href="https://github.com/boundino/tutorialTMVA/blob/master/20160608_tutorialTMVA.pdf">tutorialTMVA</a>. </li>
</ul>

<h3> mva/ </h3>
<code> macros/BDT.C </code> builds a tree with branch of BDT (or other MVA) value for each candidate.

<h3> Merging BDT trees with other trees </h3>
<p> <code>doTMVA.sh</code> <a href="https://github.com/boundino/tutorialTMVABntupleRunII/blob/master/doTMVA.sh#L149-L225">L149-L225</a>. </p>

<h3> readxml/ </h3>
<ul>
<li> <code>readxml.cc</code> is for BDT method (which can be easily developed to be applicable for other methods). It produces <a href=".backup/plots_for_readme/cmvadistribution_TMVA_BDT_PbPb_pt_10p0_15p0.pdf">BDT distribution</a>, <a href=".backup/plots_for_readme/cefficiency_TMVA_BDT_PbPb_pt_10p0_15p0.pdf">Efficiency vs. BDT cut value</a>, <a href=".backup/plots_for_readme/csignificance_TMVA_BDT_PbPb_pt_10p0_15p0.pdf">S/sqrt(S+B) vs. BDT cut value</a>, etc. <br />
</li>
<li> <code>readxml_Cuts.cc</code> is for rectangular cuts. It produces <a href=".backup/plots_for_readme/csignificance_TMVA_CutsSA_PbPb_pt_10p0_15p0.pdf">S/sqrt(S+B) vs. Signal efficiency</a>, etc. <br />
</li>
<li> Printout for <a href=".backup/plots_for_readme/fresult_TMVA_BDT_PbPb_pt_10p0_15p0.txt">BDT</a> and <a href=".backup/plots_for_readme/fresult_TMVA_CutsSA_PbPb_pt_10p0_15p0.txt">CutsSA</a>, which are also saved by the macros. <br />
</li>
</ul>


