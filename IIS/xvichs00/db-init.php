<?php // Pripojeni k databazi
	$db = mysql_connect('localhost:/var/run/mysql/mysql.sock', 'xvichs00', 'koju5gom');
	if (!$db) die('nelze se pripojit '.mysql_error());
	if (!mysql_select_db('xvichs00', $db)) die('database neni dostupna '.mysql_error());
?>
