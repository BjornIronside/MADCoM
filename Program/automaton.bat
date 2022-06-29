ECHO OFF

::madcom -mu 25 -lmbda 40 -elite 12 
ECHO madcom -mu 25 -lmbda 40 -elite 12 
ECHO madcom -mu 25 -lmbda 40 -elite 12  >> ../Program/original_params.txt

ECHO %DATE% %TIME% - egl-g2-E Run 1/1
ECHO %DATE% %TIME% - egl-g2-E Run 1/1 >> ../Program/original_params.txt
madcom "../Instances/CARP/egl-g2-E.dat" -type 30 -t 2400 -mu 25 -lmbda 40 -elite 12 
ECHO %DATE% %TIME% - F1_g-4 Run 1/1
ECHO %DATE% %TIME% - F1_g-4 Run 1/1 >> ../Program/original_params.txt
madcom "../Instances/CARP/F1_g-4.txt" -type 30 -t 2400 -mu 25 -lmbda 40 -elite 12 
ECHO %DATE% %TIME% - Hefei-10 Run 1/1
ECHO %DATE% %TIME% - Hefei-10 Run 1/1 >> ../Program/original_params.txt
madcom "../Instances/CARP/Hefei-10.txt" -type 30 -t 2400 -mu 25 -lmbda 40 -elite 12 
ECHO %DATE% %TIME% - Beijing-6 Run 1/1
ECHO %DATE% %TIME% - Beijing-6 Run 1/1 >> ../Program/original_params.txt
madcom "../Instances/CARP/Beijing-6.txt" -type 30 -t 2400 -mu 25 -lmbda 40 -elite 12 
ECHO %DATE% %TIME% - Beijing-8 Run 1/1
ECHO %DATE% %TIME% - Beijing-8 Run 1/1 >> ../Program/original_params.txt
madcom "../Instances/CARP/Beijing-8.txt" -type 30 -t 2400 -mu 25 -lmbda 40 -elite 12 
ECHO %DATE% %TIME% - S2_g-6 Run 1/1
ECHO %DATE% %TIME% - S2_g-6 Run 1/1 >> ../Program/original_params.txt
madcom "../Instances/CARP/S2_g-6.txt" -type 30 -t 2400 -mu 25 -lmbda 40 -elite 12 
ECHO %DATE% %TIME% - N5_g-5 Run 1/1
ECHO %DATE% %TIME% - N5_g-5 Run 1/1 >> ../Program/original_params.txt
madcom "../Instances/CARP/N5_g-5.txt" -type 30 -t 2400 -mu 25 -lmbda 40 -elite 12 
ECHO %DATE% %TIME% - O1_p-2 Run 1/1
ECHO %DATE% %TIME% - O1_p-2 Run 1/1 >> ../Program/original_params.txt
madcom "../Instances/CARP/O1_p-2.txt" -type 30 -t 2400 -mu 25 -lmbda 40 -elite 12 
ECHO %DATE% %TIME% - K5_g-6 Run 1/1
ECHO %DATE% %TIME% - K5_g-6 Run 1/1 >> ../Program/original_params.txt
madcom "../Instances/CARP/K5_g-6.txt" -type 30 -t 2400 -mu 25 -lmbda 40 -elite 12 
ECHO %DATE% %TIME% - O1_g-4 Run 1/1
ECHO %DATE% %TIME% - O1_g-4 Run 1/1 >> ../Program/original_params.txt
madcom "../Instances/CARP/O1_g-4.txt" -type 30 -t 2400 -mu 25 -lmbda 40 -elite 12 


