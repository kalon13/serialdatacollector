<html>
<head>
<title>SerialDataCollector - PHP Monitor</title>
</head>
<body>
<?php
	$dataset = "DataSet/Outdoor";
	$DS_newstamp = 0;
	$lastdir = "";
	$DS_dc = opendir($dataset);
	while ($DS_filename = readdir($DS_dc)) {
		if (ereg('^\.{1,2}$',$DS_filename)) continue;
		$DS_timedat = filemtime("$dataset/$fn1");
		if ($DS_timedat > $DS_newstamp) {
			$DS_newstamp = $DS_timedat;
			$lastdir = $DS_filename;
		}
	}
	
	$dir = $dataset."/".$lastdir;
	$IMG_pattern = '\.(png)$';
	$IMG_newstamp = 0;
	$IMG_newname = "";
	$IMG_dc = opendir($dir);
	while ($IMG_filename = readdir($IMG_dc)) {
	  if (ereg('^\.{1,2}$',$IMG_filename)) continue;
	  if (! ereg($IMG_pattern,$IMG_filename)) continue;
	  $IMG_timedat = filemtime("$dir/$fn");
	  if ($IMG_timedat > $IMG_newstamp) {
		$IMG_newstamp = $IMG_timedat;
		$IMG_newname = $IMG_filename;
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

/*OUTPUT DEI DATI*/

	echo "<p>DataSet: $lastdir</p>";
	echo "<p>$GPS_Data</p>";
	echo "<p>$IMU_Data</p>";

	if(IMG_newstamp!=0)
		echo "<img src=\"$dir/$newname\" />";
	else
		echo "<p>Nessuna immagine presente</p>";
?>
</body>
</html>
