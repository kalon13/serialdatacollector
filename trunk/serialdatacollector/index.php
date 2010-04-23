<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html lang="it" dir="ltr">
<head>
	<title>SerialDataCollector - PHP Monitor</title>
	<style type="text/css">
	
	</style>
</head>
<body>
<?php
	$dataset = "DataSet/Outdoor";
	$lastdir = "";
	
	/*TROVA L'ULTIMA CARTELLA CREATA*/
	$DS_newstamp = 0;
	$DS_dc = opendir($dataset);
	while ($DS_filename = readdir($DS_dc)) {
		if (ereg('^\.{1,2}$',$DS_filename)) continue;
		$DS_timedat = filemtime("$dataset/$fn1");
		if ($DS_timedat > $DS_newstamp) {
			$DS_newstamp = $DS_timedat;
			$lastdir = $DS_filename;
		}
	}
	
	/*PERCORSO DEI DATI*/
	$dir = $dataset."/".$lastdir;
	
	/*TROVA L'ULTIMA IMMAGINE*/
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
	/*VERIFICA SE ESISTE IL FILE DELLA IMU E LO LEGGE*/
	$IMU_File = $dir."/"."IMU_STRETCHED.cvs.u";
	if(file_exists ($IMU_File)) {
		$fh = fopen($IMU_File, 'r');
		$IMU_Data = fread($fh, filesize($IMU_File));
		fclose($fh);
	}
	else
		$IMU_Data = "Il file della imu non esiste!";
	/*VERIFICA SE ESISTE IL FILE DEL GPS E LO LEGGE*/
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
		echo "<img src=\"$dir/$IMG_newname\" />";
	else
		echo "<p>Nessuna immagine presente</p>";
?>
</body>
</html>
