



 Ctime = runTime.timeName();
 if (Pstream::master())
 {
	 fileName rootDir;

        if (Pstream::parRun())
        {
            rootDir = runTime.path()/"../LineResults";
        }
        else
        {
            rootDir = runTime.path()/"LineResults";
        }

        // Check to see if the turbineOutput directory exists; if not, create it.    
        if (!isDir(rootDir))
        {
            mkDir(rootDir);
        }

         //Check to see if the start time directory exists within the turbineOutput directory; if not, create it.  
        

		    outputIndex++;

	if (outputControl == "timeStep")
          {
            if (outputIndex >= outputInterval)
    	    {
	        outputIndex = 0;
	        
			
		    *thrustFile_ << Ctime <<"  "<< torque<<"  "<<power<<endl;
			//Info<<"outputIndex = "<<outputIndex<<endl;
	         }
          }
        else if (outputControl == "runTime")
        {
            if ((runTime.value() - lastOutputTime) >= outputInterval)
            {
    	    lastOutputTime += outputInterval;
			
	        
		    *thrustFile_ << Ctime <<"  "<< torque<<"  "<<power<<endl;
            }
        }
        else
        {
            Info<<"Unknow type of output"<<endl;
        }
		
 }
