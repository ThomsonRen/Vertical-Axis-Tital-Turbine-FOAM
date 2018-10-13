    // Create a post processing directory if it does not already exist. 
    fileName LineResults;


    if (Pstream::master())
    {
	
		
		if (Pstream::parRun())
        {
        LineResults = runTime.path()/"../LineResults";
        }
   
		else
       {
        LineResults = runTime.path()/"/LineResults";
        }
   
		if (!isDir(LineResults))
       {
        mkDir(LineResults);
        }


	}

	thrustFile_ = new OFstream(LineResults/"thrustFile_");
	*thrustFile_ << "Time(s)    torque(N)   power(N*m/s)" << endl;
