# Displaced Leptons BDT

This Boosted Decision Tree (BDT) takes in SUSY selectron pair production MC as signal, and ATLAS data from 2022 as background. The goal is to use measured variables from the ATLAS tracker and calorimeter to distinguish signal reconstructed by ATLAS as electrons and photons from background electron and photons.

## Suggested Reading

This BDT was made using TMVA, the [user's guide](https://root.cern.ch/download/doc/tmva/TMVAUsersGuide.pdf) is a good resource. In particular section 3: using TMVA describes the setup, with Table 2 giving a list of training configuration options.

The user's guide section 8.13 describes what a Boosted Decision Tree is and how it works.

## Input files

If you are working on xenia (the Nevis cluster) the microntuples are stored under `/data/acs2325/DispLepNtuples/MicroNtuples`.
The microntuples are stored on lxplus (CERN cluster) under  `/eos/user/a/ancsmith/DispLepNtuples/MicroNtuples`. 

In the `/signal/` directory you can find a grid of signal points with varying mass and lifetime parameters. For instance `200_0_10.root` contains events with selectron pair production, where each selectron has a mass of `200`  GeV and `10` ns mean lifetime.

In the `data` directory is a subset of ATLAS proton-proton data taken in 2022, at CoM energy 13.6TeV.

You can use `root` interactively to look inside the files:

```
root -l /eos/user/a/ancsmith/DispLepNtuples/MicroNtuples/signal/200_0_10.root 
root [0] 
Attaching file /eos/user/a/ancsmith/DispLepNtuples/MicroNtuples/signal/200_0_10.root as _file0...
(TFile *) 0x3dd5d90
root [1] .ls
TFile**		/eos/user/a/ancsmith/DispLepNtuples/MicroNtuples/signal/200_0_10.root	
 TFile*		/eos/user/a/ancsmith/DispLepNtuples/MicroNtuples/signal/200_0_10.root	
  KEY: TTree	PostSel_1e;1	PostSel_1e
  KEY: TTree	PostSel_1g;1	PostSel_1g
  KEY: TTree	PostSel_ee;1	PostSel_ee
  KEY: TTree	PostSel_eg;1	PostSel_eg
  KEY: TTree	PostSel_gg;1	PostSel_gg
```
There are 5 `TTrees` each corresponding to events in a given final state (ee = 2 electrons, eg = 1 electron,1 photon) after some trigger/kinematic selections, "PostSel".

## Description of input variables

`el1_pt:` transverse momentum of electron track. Transverse = in plane orthogonal to direction of the proton-proton beams.

`el1_eta:` pseudorapidity coordinate of leading electron (see https://en.wikipedia.org/wiki/Pseudorapidity).

`el1_maxEcell_E:` energy deposited in calorimeter cell that recieved the most energy from this electron.

`el1_dpt:` percent difference between pt of electron track and electron object. See page 27 here (https://cds.cern.ch/record/2694014/files/ATL-COM-PHYS-2019-1321.pdf).

`el1_chi2:` goodness of fit parameter for electron track fom fit to the hits that form the track.

`el1_nPix:` number of pixel layers crossed by the electron track.

The index "1" in `el1_eta` refers to the "leading" electron. This is the electron with the most transverse momentum in each event. `el2_eta` refers to "subleading" electron, which has the second most `pt` in the event.

Please, play around and add/remove input variables! You can see what is available, for instance in the two photon final state with `PostSel_gg->Print()` in interactive `root`. 

## How to run

`root -q -b -l myBDT.C`  will run the BDT and produce an `output.root` file.

In the text output you will also see a ranking of the variables input by importance, and a correlation matrix for these variables for signal and for background.

You can edit `myBDT.C`, for instance with `vim myBDT.C` or any other text editor, to change the input variables/training hyperparameters such as depth etc (table 25 and 26 of the TMVA users guide for options). There are comments within `myBDT.C` that indicate where/how to change configuration+inputs.

## How to view output

`root -l -e ’TMVA::TMVAGui("$put/in/here/your/path/to/tmva/output/file.root")`

This will bring up a GUI which is described in the user's guide. Here you can find ROC curves (https://en.wikipedia.org/wiki/Receiver_operating_characteristic), classifier score, plots of input variables, etc.
