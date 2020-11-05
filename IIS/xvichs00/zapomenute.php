<?php 
	$title = "Zapomenuté heslo";
	$isInternPage = false;
?>
<?php require('includes/header.php'); ?>

<form action="zapomenute.php" method="post" enctype="multipart/form-data" target="_self">
	<label for="email-reset">Zadejte e-mail pro obnovení hesla</label>
	<input type="email" id="email-reset" name="email-reset" required>
	<label for="login">Login</label>
	<input type="text" id="login" name="login" required>
	<input type="submit" value="Odeslat e-mail">
</form>

<?
if( isset($_POST["login"]) and isset($_POST["email-reset"])) 
{ 
		$login =    filter_var($_POST["login"],    FILTER_SANITIZE_STRING);
		$email = filter_var($_POST["email-reset"], FILTER_SANITIZE_STRING);

		//nacitanie dat z databaze
		$query1="SELECT * FROM pracovnik_banky WHERE login='".$login."' AND email='".$email."'";
		$result1 = @mysql_query($query1, $db); 
		$zhoda = false;
		while ($data = mysql_fetch_array($result1, MYSQL_ASSOC)) 
		{
			$zhoda = true;
			$heslo = $data["heslo"];
			$msg = 'Vaše ztracené heslo je: '.$heslo.'.';
			$msg = wordwrap($msg, 70);
			$headers = "From: help@nasabanka.com\nContent-Type: text/html; charset=UTF-8\n";
			mail($email, "Ztracené heslo", $msg, $headers);
		}
		

		if($zhoda == false) 
		{
			echo "<p>Nezadali jste správnou kombinaci mailu a loginu.</p>";
		} else {
			echo "<p>Heslo bylo odesláno na váš email.</p>";
		}
	}
?>
<?php require('includes/footer.php'); ?>
