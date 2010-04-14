<html>
<head>
<title>SerialDataCollector - PHP Monitor</title>
</head>
<body>
<?php
	$dataset = "DataSet/Outdoor";
	$newstamp1 = 0;
	$lastdir = "";
	$dc1 = opendir($dataset);
	while ($fn1 = readdir($dc1)) {
		if (ereg('^\.{1,2}$',$fn1)) continue;
		$timedat1 = filemtime("$dataset/$fn1");
		if ($timedat1 > $newstamp1) {
			$newstamp1 = $timedat1;
			$lastdir = $fn1;
		}
	}
	$dir = $dataset."/".$lastdir;
	$pattern = '\.(png)$';
	$newstamp = 0;
	$newname = "";
	$dc = opendir($dir);
	while ($fn = readdir($dc)) {
	  if (ereg('^\.{1,2}$',$fn)) continue;
	  if (! ereg($pattern,$fn)) continue;
	  $timedat = filemtime("$dir/$fn");
	  if ($timedat > $newstamp) {
		$newstamp = $timedat;
		$newname = $fn;
	  }
	}

	$IMU_File = $dir."/"."IMU_STRETCHED.cvs.u";
	if(file_exists ($IMU_File)) {
		$fh = fopen($IMU_File, 'r');
		$IMU_Data = fread($fh, filesize($IMU_File));
		fclose($fh);
	}
	else
		$IMU_Data = "Il file della imu non esiste!";
	$GPS_File = $dir."/"."GPS.cvs.u";
	if(file_exists($GPS_File)) {
		$fh = fopen($GPS_File, 'r');
		$GPS_Data = fread($fh, filesize($GPS_File));
		fclose($fh);
	}
	else
	$GPS_Data = "Il file del gps non esiste";
?>
<p><?=$GPS_Data?></p><p><?=$IMU_Data?></p><br><img src="<?=$dir?>/<?=$newname?>" />
</body>
</html>
