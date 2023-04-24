function sname()
{
  local session_name="";
  week=`date +%u`;
  session=`date +%H`;

  if [ $week == 1 ]
  then
    if [ $session -le 12  ]
    then
      session_name="ML1";
    else
      session_name="AL1";
    fi;
  elif [ $week == 2 ]
  then
    if [ $session -le 12  ]
    then
      session_name="ML2";
    else
      session_name="AL2";
    fi
  elif [ $week == 3 ]
  then
    if [ $session -le 12  ]
    then
      session_name="ML3";
    else
      session_name="AL3";
    fi
  elif [ $week == 4 ]
  then
    if [ $session -le 12  ]
    then
      session_name="ML4";
    else
      session_name="AL4";
    fi
  elif [ $week == 5 ]
  then
    if [ $session -le 12  ]
    then
      session_name="ML5";
    else
      session_name="AL5";
    fi
  elif [ $week == 6 ]
  then
#   echo "Today is Saturday and there is no lab session";
#session_name="ML6";
    if [ $session -le 12  ]
    then
      session_name="ML6";
    else
      session_name="AL6";
    fi
  elif [ $week == 7 ]
  then
#   echo "Today is Sunday and there is no lab session";
#   session_name="ML7";
    if [ $session -le 12  ]
    then
      session_name="ML7";
    else
      session_name="AL7";
    fi
  fi;
  echo $session_name;
}

iitg_cube="`seq 2 108` `seq 110 120`"


for i in `seq 1 5`
do
  /usr/bin/rm -rf /home/iitg/problem/ML$i
  /usr/bin/rm -rf /home/iitg/problem/AL$i
done;


session_name=$(sname);
name=`ls /home/iitg/regular-labs/ -t`;
wname=`echo $name | cut -d' ' -f1`;
dir_name=$wname"/"$session_name;
echo "cp -r $dir_name /home/iitg/problem";

cp -r /home/iitg/regular-labs/$dir_name /home/iitg/problem;

hrs=`date +%H`
mins=`date +%M`
dt=`date +%F`
log_file_name="/home/iitg/scripts/ta-scripts/copy-log/"$dir_name"-"$dt"-"$hrs"-"$mins".log";

if [ $# == 1 ]
then
  i=$1;
  echo "Copying lecture slides to machine: $i" 2> /dev/null ;
  scp -r /home/iitg/lectures guest@172.16.6.$i:~/ 2> /dev/null ;
  echo "Copying problem directory to machine: $i" 2> /dev/null ;
  scp -r /home/iitg/problem/ guest@172.16.6.$i:~/ 2> /dev/null ;
  if [ $? -eq 0 ]
  then
    echo "Copied problem directory to machine $i" >> $log_file_name;
  else
    echo "Copying problem directory to machine $i FAILED" >> $log_file_name;
  fi;
  scp -r /home/iitg/regular-labs/week01/cs110-introduction.pdf guest@172.16.6.$i:~/ 2> /dev/null ;
else
  for i in `echo $iitg_cube`
  do
    echo "Copying lecture slides to machine: $i" 2> /dev/null ;
    scp -r /home/iitg/lectures/ guest@172.16.6.$i:~/ 2> /dev/null ;
    echo "Copying problem directory to machine: $i" 2> /dev/null ;
    scp -r /home/iitg/problem/ guest@172.16.6.$i:~/ 2> /dev/null ;
    if [ $? -eq 0 ]
    then
      echo "Copied problem directory to machine $i" >> $log_file_name;
    else
      echo "Copying problem directory to machine $i FAILED" >> $log_file_name;
    fi;
    scp -r /home/iitg/regular-labs/week01/cs110-introduction.pdf guest@172.16.6.$i:~/ 2> /dev/null ;
  done
fi

