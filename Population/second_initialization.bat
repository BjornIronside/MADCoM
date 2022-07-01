ECHO OFF

::popanalysis -mu 100 -hdf 1 -psf "second" -selmet 2 
ECHO popanalysis -mu 100 -hdf 1 -psf "second" -selmet 2 
ECHO popanalysis -mu 100 -hdf 1 -psf "second" -selmet 2  >> ../Population/medoid_initialization.txt

ECHO %DATE% %TIME% - Hefei-10 Run 1/1
ECHO %DATE% %TIME% - Hefei-10 Run 1/1 >> ../Population/medoid_initialization.txt
popanalysis "../Instances/CARP/Hefei-10.txt" -type 30 -t 2400 -mu 100 -hdf 1 -psf "second" -selmet 2 
ECHO %DATE% %TIME% - Beijing-6 Run 1/1
ECHO %DATE% %TIME% - Beijing-6 Run 1/1 >> ../Population/medoid_initialization.txt
popanalysis "../Instances/CARP/Beijing-6.txt" -type 30 -t 2400 -mu 100 -hdf 1 -psf "second" -selmet 2 
ECHO %DATE% %TIME% - Beijing-8 Run 1/1
ECHO %DATE% %TIME% - Beijing-8 Run 1/1 >> ../Population/medoid_initialization.txt
popanalysis "../Instances/CARP/Beijing-8.txt" -type 30 -t 2400 -mu 100 -hdf 1 -psf "second" -selmet 2 
ECHO %DATE% %TIME% - S2_g-6 Run 1/1
ECHO %DATE% %TIME% - S2_g-6 Run 1/1 >> ../Population/medoid_initialization.txt
popanalysis "../Instances/CARP/S2_g-6.txt" -type 30 -t 2400 -mu 100 -hdf 1 -psf "second" -selmet 2 
ECHO %DATE% %TIME% - N5_g-5 Run 1/1
ECHO %DATE% %TIME% - N5_g-5 Run 1/1 >> ../Population/medoid_initialization.txt
popanalysis "../Instances/CARP/N5_g-5.txt" -type 30 -t 2400 -mu 100 -hdf 1 -psf "second" -selmet 2 
ECHO %DATE% %TIME% - O1_p-2 Run 1/1
ECHO %DATE% %TIME% - O1_p-2 Run 1/1 >> ../Population/medoid_initialization.txt
popanalysis "../Instances/CARP/O1_p-2.txt" -type 30 -t 2400 -mu 100 -hdf 1 -psf "second" -selmet 2 
ECHO %DATE% %TIME% - K5_g-6 Run 1/1
ECHO %DATE% %TIME% - K5_g-6 Run 1/1 >> ../Population/medoid_initialization.txt
popanalysis "../Instances/CARP/K5_g-6.txt" -type 30 -t 2400 -mu 100 -hdf 1 -psf "second" -selmet 2 
ECHO %DATE% %TIME% - O1_g-4 Run 1/1
ECHO %DATE% %TIME% - O1_g-4 Run 1/1 >> ../Population/medoid_initialization.txt
popanalysis "../Instances/CARP/O1_g-4.txt" -type 30 -t 2400 -mu 100 -hdf 1 -psf "second" -selmet 2 


