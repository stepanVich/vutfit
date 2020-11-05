<?php
	$title = "Vnitřní systém";
	$isInternPage = true;
?>
<?php require('includes/header.php'); ?>

<p>Vítejte v systému. Pokud neobnovíte stránku do 5 minut budete automaticky odhlášeni.</p>
<p>Vaše role: <?php echo ($_SESSION["role"] == "pracovnik" ? "Pracovník" : "Administrátor systému"); ?></p>

<?php 
include("includes/pracovnik.php");
?>
<?php require('includes/footer.php'); ?>
