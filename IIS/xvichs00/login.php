<?php 
	$title = "Přihlášení";
	$isInternPage = false;
?>
<?php require('includes/header.php'); ?>

<form class = login-form" action="login.php" method="post" enctype="multipart/form-data" target="_self">
	<label for="login">Login</label>
	<input type="text" id="login" name="login" required><br>
	<label for="password">Heslo</label>
	<input type="password" name="password" id="password" required><br>
	<input type="submit" value="Přihlášení">
</form>
<?php if(isset($_SESSION["loginFailed"])) {
	echo "<p>Špatně zadán login nebo heslo.</p>";
	unset($_SESSION["loginFailed"]);
}
?>

<?php require('includes/footer.php'); ?>
