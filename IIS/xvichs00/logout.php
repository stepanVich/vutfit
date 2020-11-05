<?php
        $title = "Odhlášení";
        $isInternPage = true;
?>
<?php 
	include('includes/header.php'); 
	destroySession();
?>

<p>Byli jste úspěšně odhlášení. Pokračujte na <a href="index.php">hlavní stránku</a>.

<?php include('includes/footer.php'); ?>
