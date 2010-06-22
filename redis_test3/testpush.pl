                use Encode;
                open (FILE,"60k.txt") || die "can't open the file 60k.txt";
                my @date=<FILE>;
               # print "@date";
               # my $date=split (/\s+/,@date);
               # print "$date\n";
                my $queue_name="abc";
                for($i=0;$i<10000;$i++){
                      my $result=`./redis-cli lpush $queue_name @date`;
                      print "$result";

                #       eval{
                #       $code=Encode("utf8",$result);
                #       };
                #       if ($@){
                #       print "not utf8\n";
#}

#                       eval{
#                        $code=Encode("gb2312",$result);
#                        };
#                        if ($@){
#                        print "not gbk\n";
#}



#                     eval{
#                        $code=Encode("big5",$result);
#                        };
#                        if ($@){
#                        print "not big5\n";
#}


                      while(!$result){
                          print "push $i message again  ";
                          $result=`./redis-cli qpush $queue_name $i`;
                         print "++++++++++++++++++++$result";
		      }
}

