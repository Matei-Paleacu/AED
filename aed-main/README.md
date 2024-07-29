# COMP3004 AED Team Project
BLAHAJ Electronics (Shork Electronics) - AED Device
## Team Members
Rayyan Alam  
Matei Paleacu  
Mac Zhang  
Patrick Song  
## Deliverable Split
Use Case: **Matei Paleacu & Rayyan Alam** & Patrick Song

Tracebility Matrix: **Matei Paleacu & Rayyan Alam**

UML class diagram: **Matei Paleacu**

Sequence diagrams **Matei Paleacu & Rayyan Alam**

State diagrams **Matei Paleacu & Mac Zhang**

Qt C++ Headers & Device Backend **Patrick Song**

Qt C++ Backend **Patrick Song & Rayyan Alam & Matei Paleacu & Mac Zhang**

Qt C++ Frontend **Mac Zhang & Rayyan Alam & Patrick Song**  

Code control flow & FE/BE SoC **Patrick Song**  

Video **Mac Zhang**  

README **Mac Zhang**  

## About
Software simulation made in Qt Creator in C++17 that mimics the core functionality of an AED Plus device utilizing a Model-View-Controller design pattern    
![AED Plus](https://images.prismic.io/aed-advantage/9e1dc7e7-31b9-475f-8e5e-2ad7b00f5087_8513-001103-01-02-24.jpg?auto=compress%2Cformat&rect=0%2C0%2C3000%2C3000&w=560)
## Demo Link
https://youtu.be/FvIXuPpfdmY 
## Files
├── aed.pro                          # UI project file  
├── include/shork/cs                 # Header files files (alternatively `Headers`)  
├── src                              # Source files (alternatively `Sources`)  
├── form                             # UI form files (alternatively `Forms`)  
├── img                              # Image folder (alternatively `Resources`)  
└── README.md  
## Source files
The actual source files of a software project are usually stored inside the
`src` folder.  
This contains `aed_device.cpp`, `main.cpp`, `mainwindow.cpp`, `state_manager.cpp`
## Header files
.  
    ├── ...  
    ├── include/shork/cs       **# namespace**  
    │   ├── bit                **# contains self test for AED**  
    │   │   └── self_test.h  
    │   ├── intervention       **# contains compression info for AED**   
    │   │   └── compression.h  
    │   ├── model              **# contains device info for AED**   
    │   │   └── device_data.h  
    │   ├── sensor             **# contains ecg info for AED**    
    │   │   └── ecg.h  
    │   ├── aed_device.h  
    │   ├── mainwindow.h  
    │   └── state_manager.h                
    └── ...  
## Images
`pic.png` 1-5 are all pictures corresponding to AED plus procedure.     
`VF.png`, `VT.png`, `AS.png`, `SR.png` corresponds to each heart signal outlined in the specifications  
`Heart.png` is free use clipart of a heart
## Installation
Install and run the code in the [COMP3004](https://carleton.ca/scs/tech-support/virtual-machines/#current-list-virtualmachines) virtual machine.    
Assuming file is already unzipped run Qmake, then build and run the code.    
## Usage
Watch the demo video on how testing was done and replicate the procedure.  
## Learn more
Learn more about Qt **here**: https://doc.qt.io/
