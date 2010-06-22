for($i=1;$i<1000;$i++){	 	 print  `./redis-cli lpop abc` ;		 print "  ";           	 unless ($i%50){                     print "\n";                  }}
