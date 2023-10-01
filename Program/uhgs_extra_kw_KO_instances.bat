ECHO OFF

::madcom -hdf 0 -mutprob 0 -mu 25 -lmbda 40 -elite 12 -div 3 
ECHO madcom -hdf 0 -mutprob 0 -mu 25 -lmbda 40 -elite 12 -div 3 
ECHO madcom -hdf 0 -mutprob 0 -mu 25 -lmbda 40 -elite 12 -div 3  >> uhgs_extra_kw_KO_instances.txt

ECHO %DATE% %TIME% - O1_p-2 Run 1/1
ECHO %DATE% %TIME% - O1_p-2 Run 1/1 >> uhgs_extra_kw_KO_instances.txt
madcom "../Instances/CARP/O1_p-2.txt" -type 30 -t 1162 -hdf 0 -mutprob 0 -mu 25 -lmbda 40 -elite 12 -div 3 

ECHO %DATE% %TIME% - O1_p-4 Run 1/1
ECHO %DATE% %TIME% - O1_p-4 Run 1/1 >> uhgs_extra_kw_KO_instances.txt
madcom "../Instances/CARP/O1_p-4.txt" -type 30 -t 1162 -hdf 0 -mutprob 0 -mu 25 -lmbda 40 -elite 12 -div 3 

ECHO %DATE% %TIME% - K2_g-4 Run 1/1
ECHO %DATE% %TIME% - K2_g-4 Run 1/1 >> uhgs_extra_kw_KO_instances.txt
madcom "../Instances/CARP/K2_g-4.txt" -type 30 -t 1358 -hdf 0 -mutprob 0 -mu 25 -lmbda 40 -elite 12 -div 3 