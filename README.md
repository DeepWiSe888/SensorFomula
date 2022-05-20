 # Sensor Formula
#### A Platform For Sensors Data Fusion and Collaborative Working. 
&nbsp;

# 1. Introducion 

We do have many types of sensor devices and labeled data, such as cameras, radar, lidar, thermal, ultrasound, etc. These devices and production data are expect to work collaboration, including time synchronization, coordinate system unification, data normalization, and so on.
On the other hand, we also have sensing data processing code libraries, we expect the data and processing codes can be highly abstract and reusable.

So we design this sensor devices and data abstract platform, it resolved these above problems. We named it 'Sensor Formula'(SF), cause we known that the advent of math symbols and formulas unified math problems in diffrent regions, and now we are doing the same things. 
#### &nbsp;

  
# 2. Features 

Now Sensor Formula is at step one, abbr as SF1. Here is a list of SF1's more intresting features: 
- Abstract storage and transmission for various different sensor data.
- Universal data unit structure based on sensor tagging. 
- Integrate sdk interfaces for common sensor devices.(Provided by AiWise Inc.)
- Common BLAS library.
- Common Sensing data Processing Algorithms.
- Integrate DAG(Directed Acyclic Graph) software system.
- Highly abstract formulaic programming.*1
###### &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*1: maybe at SF2 version. 
- Parallel tasks working.
- Support embedded systems. Portable to any platform with c/c++ compiler.
- Language C/C++ based, Python3.x interfaces.
- Release under the GNU General Public License version 3(GPL3.0).

  
#### &nbsp;

# 3. Components

1. Base Libs  
1.1 Universal Data Definitions  
1.2 BLAS Libraries  
&nbsp; &nbsp; 1.2.1 Basic Algebra  
&nbsp; &nbsp; 1.2.2 Filters  
&nbsp; &nbsp; 1.2.3 T/F Domain Transforms  
&nbsp; &nbsp; 1.2.4 Coordinate Transforms  
1.3 DAG System  
1.4 Application Math Works   
&nbsp; &nbsp; 1.4.1 Vital Sign Detection  
&nbsp; &nbsp; 1.4.2 3D Clustering and Object Tracking  
&nbsp; &nbsp; 1.4.3 Classifiers  
1.5 Utils   
&nbsp; &nbsp; 1.5.1 Universal TCP    
&nbsp; &nbsp; 1.4.2 File Operation  
2. ToolBox  
2.1 Sensor Devices Interfaces  
2.2 Examples  
2.3 UI Box  
3. Hardware Support
4. Documents


