<p>
	Uživatelské rozhraní pro editaci profilu a správu a vyhledávání klientů v databázi bankovního systému.	
</p>
<p>
	<ul>
		<li><a href="?profilePage">Profilové informace pracovníka</a>
		<li><a href="?searchPage">Nalézt klienta</a>
		<li><a href="?addClient">Přidat nového klienta</a>
		<?php if($_SESSION["role"] == "admin") {
		?>
			<li><a href="?listWorkers">Vypsat pracovníky</a></li>
			<li><a href="?addWorker">Přidat pracovníka</a></li>	
		<?php
		} ?>
	</ul>
</p>
	<?php 
		// Profile info modul
		if(isset($_GET["profilePage"])) {

			// Nacteni dat z databaze
			$query1="SELECT * FROM pracovnik_banky WHERE id_pracovnik = '".$_SESSION["row"]["id_pracovnik"]."'";
			$result1 = @mysql_query($query1, $db); 
			while ($data = mysql_fetch_array($result1, MYSQL_ASSOC)) {
				// Vypsani informaci o pracovnikovi
				?>
				<table>
					<tr><td>Jméno</td><td><?php    echo $data["jmeno"] ?></td></tr>
					<tr><td>Příjmení</td><td><?php echo $data["prijmeni"]?></td></tr>
					<tr><td>Adresa</td><td><?php   echo $data["adresa"] ?></td></tr>
					<tr><td>Telefon</td><td><?php  echo $data["telefon"]?></td></tr>
					<tr><td>E-mail</td><td><?php   echo $data["email"]?></td></tr>
				</table>
				<a href='?editProfilePage'>Editovat</a>
				<?php
			}			
		}

		//profile info edit
		else if(isset($_GET["editProfilePage"])) {

			// Validace a ulozeni hodnot
			if(isset($_POST["jmeno"])) {
				// Osetreni vstupnich hodnot
				// Ochrana proti SQL injekci
				$jmeno =    filter_var($_POST["jmeno"],    FILTER_SANITIZE_STRING);
				$prijmeni = filter_var($_POST["prijmeni"], FILTER_SANITIZE_STRING);
				$adresa = filter_var($_POST["adresa"], FILTER_SANITIZE_STRING);
				$telefon = filter_var($_POST["telefon"], FILTER_SANITIZE_STRING);
				$email= filter_var($_POST["email"], FILTER_SANITIZE_STRING);
				$sql = "UPDATE pracovnik_banky ";
				$sql .= "SET jmeno='".$jmeno."', ";
				$sql .= "prijmeni='".$prijmeni."', ";
				$sql .= "adresa='".$adresa."', ";
				$sql .= "telefon='".$telefon."', ";
				$sql .= "email='".$email."' ";
				$sql .= " WHERE id_pracovnik='".$_SESSION["row"]["id_pracovnik"]."'";
				// Update informaci
				@mysql_query($sql, $db);
			}	

			// Nacteni dat z databaze
			$query1="SELECT * FROM pracovnik_banky WHERE id_pracovnik = '".$_SESSION["row"]["id_pracovnik"]."'";
			$result1 = @mysql_query($query1, $db); 
			$dataOut;
			while ($data = mysql_fetch_array($result1, MYSQL_ASSOC)) {
				$dataOut = $data;
				// Vypsani informaci o pracovnikovi
				?>
				<form action="?editProfilePage" method="post">
					<table>
						<tr><td>Jméno*</td><td><input type="text" value="<?php    echo $data["jmeno"] ?>" name="jmeno" required></td></tr>
						<tr><td>Příjmení*</td><td><input type="text" value="<?php echo $data["prijmeni"]?>" name="prijmeni" required></td></tr>
						<tr><td>Adresa</td><td><input type="text" value="<?php   echo $data["adresa"] ?>" name="adresa"></td></tr>
						<tr><td>Telefon</td><td><input type="number" value="<?php  echo $data["telefon"]?>" name="telefon"></td></tr>
						<tr><td>E-mail</td><td><input type="email" value="<?php  echo $data["email"]?>" name="email"></td></tr>
					</table>
					<input type="submit">
				</form>
				<?php
			}		
		}

		// Search page modul
		else if(isset($_GET["searchPage"])) {
			$s = "";
			if(isset($_POST["searchString"])) {
				$s=filter_var($_POST["searchString"], FILTER_SANITIZE_STRING);
			}
			?>

				<form action="?searchPage" method="post">
					<label for="search-box">Vyhledat klienta</label></br>
					<input placeholder="Zadejte rodné číslo, příjmení, jméno, login, telefon pro identifikaci" id="search-box" type="text" value="<?php echo $s; ?>" name="searchString">
					<input type="submit" value="Vyhledat">
				</form>
			<?php
				if(isset($_POST["searchString"])) {	
					// Vyhledej zaznamy mezi klienty
					$query="SELECT * FROM klient WHERE jmeno='{$s}' OR prijmeni='{$s}' OR email='{$s}' OR r_cislo='{$s}' OR cislo_op='{$s}' OR telefon='{$s}'"; 
					$result = @mysql_query($query, $db); 
					$nalez = false;
					echo '<div class="seach-results">';
					while ($data = mysql_fetch_array($result, MYSQL_ASSOC)) {
						// Vypis prijata data
						$nalez = true;
						?>
						
							<div class="zaznam">
								<?php echo "<a href=?clientId={$data["r_cislo"]}>{$data["jmeno"]} {$data["prijmeni"]}</a>, RČ: {$data["r_cislo"]}"; ?>
							</div>

						<?php
						
					}
					if($nalez == false) {
						echo "Nebyl nalezen žádný záznam";
					}
					echo "</div>";
				}
		}	

		//client information
		else if(isset($_GET["clientId"]))
		{
			// Ziskat informace o klientovi z databaze a vypsat je
			// Nacteni dat z databaze
			$clientId=filter_var($_GET["clientId"], FILTER_SANITIZE_STRING);
			$query1="SELECT * FROM klient WHERE r_cislo = '".$_GET["clientId"]."'";
			$result1 = @mysql_query($query1, $db); 
			while ($data = mysql_fetch_array($result1, MYSQL_ASSOC)) 
			{
				// Vypsani informaci o klientovi
				?>
				<table>
					<tr><td>Jméno</td><td><?php    echo $data["jmeno"] ?></td></tr>
					<tr><td>Příjmení</td><td><?php echo $data["prijmeni"]?></td></tr>
					<tr><td>Adresa</td><td><?php   echo $data["adresa"] ?></td></tr>
					<tr><td>Telefon</td><td><?php  echo $data["telefon"]?></td></tr>
					<tr><td>E-mail</td><td><?php   echo $data["email"]?></td></tr>
					<tr><td>R. Č.</td><td><?php    echo $data["r_cislo"]?></td></tr>
					<tr><td>Číslo OP.</td><td><?php echo $data["cislo_op"]?></td></tr>
				</table>
				<?php echo "<a href=?setClientId={$data["r_cislo"]}>Editovat</a>"; ?>
				<?php echo "<a href=?deleteClientId={$data["r_cislo"]}>Zmazat</a>"; ?>
				<?php
			}

			// Smaz ucet
			if(isset($_GET["deleteAccount"])) {
				$c_uctu = filter_var($_GET["deleteAccount"], FILTER_SANITIZE_STRING);
				// Smazat ucet
				$sql1 = "DELETE FROM bankovni_ucet WHERE c_uctu = {$c_uctu}";
				$sql2 = "DELETE FROM disponuje WHERE vlastni = {$c_uctu}";
				$sql3 = "DELETE FROM bezny WHERE c_uctu = {$c_uctu}";
				$sql4 = "DELETE FROM uverovy WHERE c_uctu = {$c_uctu}";
				$sql5 = "DELETE FROM sporici WHERE c_uctu = {$c_uctu}";
				mysql_query($sql1, $db);
				mysql_query($sql2, $db);	
				mysql_query($sql3, $db);	
				mysql_query($sql4, $db);	
				mysql_query($sql5, $db);	
			}

			// Pridej penize na ucet
			if(isset($_POST["accountId"])) {
				$accountId = filter_var($_POST["accountId"], FILTER_SANITIZE_STRING);
				$plusVal = filter_var($_POST["value"], FILTER_SANITIZE_NUMBER_INT);
				$query = "SELECT * FROM bankovni_ucet WHERE c_uctu = {$accountId}";
				$result = mysql_query($query, $db);
				$data = mysql_fetch_array($result, MYSQL_ASSOC);
				$val = $data["stav_uctu"];
				$val += $plusVal;
				$queryInsert = "UPDATE bankovni_ucet SET stav_uctu = '{$val}' WHERE c_uctu = {$accountId}";
				mysql_query($queryInsert, $db);
			}

			// Ziskat informace o klientovych uctech a vypsat je + vyber a vklad penez + nastaveni limitu
			?>
			<h2>Seznam účtů</h2>

			<h3>Bežné účty:</h3>
			<?
			$query2 = "SELECT * FROM bezny, bankovni_ucet, disponuje, klient WHERE bezny.c_uctu=bankovni_ucet.c_uctu AND bankovni_ucet.c_uctu=disponuje.vlastni AND disponuje.disponujici=klient.cislo_op AND klient.r_cislo='{$clientId}'"; 
			$result2 = @mysql_query($query2, $db); 
			echo "<table>";
			while ($data = mysql_fetch_array($result2, MYSQL_ASSOC)) 
			{
				echo "<tr><td>".$data["c_uctu"]. "</td><td>". $data["stav_uctu"]. " Kč</td></tr>";
				?>
					
						<form action="?clientId=<?php echo $clientId ?>" method="post">
							<tr><td><input type="text" name="value"></td><td><input type="submit" value="Úprava hodnoty"></td></tr>
							<input type="hidden" name="accountId" value="<?php echo $data['c_uctu']; ?>">
						</form>
						<tr><td><a href="?clientId=<?php echo $clientId; ?>&deleteAccount=<?php echo $data['c_uctu']; ?>">Smazat účet</a></td></tr>
						<tr><td><a href="?moneyTransfer=<?php echo $data["c_uctu"] ?>">Odoslať peniaze<br><br></a></td></tr>
				<?php
			}
			echo "</table>";

			?><h3>Spořící účty:</h3><?
			$query3 = "SELECT * FROM sporici, bankovni_ucet, disponuje, klient WHERE sporici.c_uctu=bankovni_ucet.c_uctu AND bankovni_ucet.c_uctu=disponuje.vlastni AND disponuje.disponujici=klient.cislo_op AND klient.r_cislo='{$clientId}'"; 
			$result3 = @mysql_query($query3, $db); 
			echo "<table>";

			while ($data = mysql_fetch_array($result3, MYSQL_ASSOC)) 
			{
				echo "<tr><td>".$data["c_uctu"]. "</td><td>". $data["stav_uctu"]. " Kč</td></tr>";
				?>
					
						<form action="?clientId=<?php echo $clientId ?>" method="post">
							<tr><td><input type="text" name="value"></td><td><input type="submit" value="Úprava hodnoty"></td></tr>
							<input type="hidden" name="accountId" value="<?php echo $data['c_uctu']; ?>">
						</form>
						<tr><td><a href="?clientId=<?php echo $clientId; ?>&deleteAccount=<?php echo $data['c_uctu']; ?>">Smazat účet</a></td></tr>
						<tr><td><a href="?moneyTransfer=<?php echo $data["c_uctu"] ?>">Odoslať peniaze<br><br></a></td></tr>				<?php
			}
			echo "</table>";

			?><h3>Úvěrové účty:</h3><?
			$query4 = "SELECT * FROM uverovy, bankovni_ucet, disponuje, klient WHERE uverovy.c_uctu=bankovni_ucet.c_uctu AND bankovni_ucet.c_uctu=disponuje.vlastni AND disponuje.disponujici=klient.cislo_op AND klient.r_cislo='{$clientId}'"; 
			$result4 = @mysql_query($query4, $db); 
			echo "<table>";
			while ($data = mysql_fetch_array($result4, MYSQL_ASSOC)) 
			{
				echo "<tr><td>".$data["c_uctu"]. "</td><td>". $data["stav_uctu"]. " Kč</td></tr>";
				?>
					
						<form action="?clientId=<?php echo $clientId ?>" method="post">
							<tr><td><input type="text" name="value"></td><td><input type="submit" value="Úprava hodnoty"></td></tr>
							<input type="hidden" name="accountId" value="<?php echo $data['c_uctu']; ?>">
						</form>
						<tr><td><a href="?clientId=<?php echo $clientId; ?>&deleteAccount=<?php echo $data['c_uctu']; ?>">Smazat účet</a></td></tr>
						<tr><td><a href="?moneyTransfer=<?php echo $data["c_uctu"] ?>">Odoslať peniaze<br><br></a></td></tr>				<?php
				
			}
			echo "</table>";
			?><a href="?newAccount=<?php echo $clientId; ?>">Přidat účet</a></br><?
			
		}

		//vyber do ktorej banky chceme peniaze previest
		else if(isset($_GET["moneyTransfer"]))
		{
			$cisloUctu=filter_var($_GET["moneyTransfer"], FILTER_SANITIZE_STRING);
			?>
				<form action="?transferTo=<?php echo $cisloUctu ?>" method="post">
				  <tr><td><input type="radio" name="banka" value="tato" checked> Na účet v tejto banke<br></td></tr>
				  <tr><td><input type="radio" name="banka2" value="cudzia"> Na účet v inej banke<br></td></tr>
				  <tr><td><input type="submit"><br><br>
				</form>
			<?
		}

		//vyber uctu a sumy a prevedenie transakcie
		else if(isset($_GET["transferTo"]))
		{
			if(isset($_POST["accountId"])) 
			{
				$plusSuma=  filter_var($_POST["suma"],      FILTER_SANITIZE_NUMBER_INT);
				$accountId = filter_var($_POST["accountId"], FILTER_SANITIZE_STRING);
				$query = "SELECT * FROM bankovni_ucet WHERE c_uctu = {$accountId}";
				$result = mysql_query($query, $db);
				$data = mysql_fetch_array($result, MYSQL_ASSOC);
				$val = $data["stav_uctu"];
				$val -= $plusSuma;
				$queryInsert = "UPDATE bankovni_ucet SET stav_uctu = '{$val}' WHERE c_uctu = {$accountId}";
				mysql_query($queryInsert, $db);
				if(mysql_query($queryInsert, $db)) 
				{
					echo "Prevod prebehol úspešne.";
				} 
				else 
				{
					echo "Prevod sa nepodaril.";
				}
			}


			$query1="SELECT * FROM bankovni_ucet WHERE c_uctu = '".$_GET["transferTo"]."'";
			$result1 = @mysql_query($query1, $db); 
			while ($data = mysql_fetch_array($result1, MYSQL_ASSOC)) 
			{
				?>
				<form action="?transferTo= <?php   echo $data["c_uctu"] ?>" method="post">
					<table>
						<tr><td>Číslo dotovaného účtu:* <input type="text" name="ucet" required></td></tr>
						<tr><td>Suma:* <input type="number" name="suma" min="0" required></td></tr>
						<input type="hidden" name="accountId" value="<?php echo $data['c_uctu']; ?>">
					</table>
					<input type="submit" value="Potvrdiť">
				</form>
				<?php
			}

		}

		//pridanie noveho uctu
		else if(isset($_GET["newAccount"]))
		{
			$clientId=filter_var($_GET["newAccount"], FILTER_SANITIZE_STRING);
			$query1="SELECT * FROM klient WHERE r_cislo = '".$clientId."'";
			$result1 = mysql_query($query1, $db); 
			$data = mysql_fetch_array($result1, MYSQL_ASSOC);
			if(isset($_POST["c_uctu"]))
			{
				$c_uctu=        filter_var($_POST["c_uctu"], 		FILTER_SANITIZE_STRING);
				$stav_uctu=	    filter_var($_POST["stav_uctu"],     FILTER_SANITIZE_STRING);
				$limit_uctu=	filter_var($_POST["limit_uctu"],     FILTER_SANITIZE_STRING);
				$typ_uctu =     filter_var($_POST["typ"], FILTER_SANITIZE_STRING);

				$sql1 = "INSERT INTO bankovni_ucet(c_uctu, stav_uctu) VALUES ('{$c_uctu}', '{$stav_uctu}')";
				$sql2 = "INSERT INTO disponuje(disponujici, vlastni, limit_uctu) VALUES ('{$data["cislo_op"]}', '{$c_uctu}', '{$limit_uctu}')";
				$sql3 = "INSERT INTO {$typ_uctu}(c_uctu) VALUES ('{$c_uctu}')";
				$true = mysql_query($sql1, $db);
				if($true and mysql_query($sql2, $db) and mysql_query($sql3, $db) ) {
					header("Location: ?clientId={$clientId}");
				} else {
					echo "Nepodařilo se vytvořit účet.";
				}
				
			}

			?>
				<form action="?newAccount=<?php echo $clientId; ?>" method="post">
					<table>
						<tr><td>Číslo účtu:* </td><td><input type="number" name="c_uctu" required></td></tr>
						<tr><td>Stav účtu:* </td><td><input type="number" name="stav_uctu" required></td></tr>
						<tr><td>Limit účtu:* </td><td><input type="number" name="limit_uctu" required></td></tr>
						<tr><td>Typ účtu: </td><td>
						<select name="typ">
							<option value="bezny">Běžný</option>
							<option value="uverovy">Ůvěrový</option>
							<option value="sporici">Spořící</option>
						</select></td></tr>

					</table>
					<input type="submit" value="Založit účet">
				</form>
			<?php
		}

		//zmazanie klienta
		else if(isset($_GET["deleteClientId"])) 
		{

				$sql = "DELETE FROM klient ";
				$sql .= "WHERE r_cislo='".$_GET["deleteClientId"]."'";
				@mysql_query($sql, $db);	
		}

		//client edit
		else if(isset($_GET["setClientId"])) 
		{
			if(isset($_POST["jmeno"])) 
			{
				$rCislo  = filter_var($_GET["setClientId"], FILTER_SANITIZE_STRING);
				$jmeno   = filter_var($_POST["jmeno"],    FILTER_SANITIZE_STRING);
				$prijmeni= filter_var($_POST["prijmeni"], FILTER_SANITIZE_STRING);
				$adresa  = filter_var($_POST["adresa"],   FILTER_SANITIZE_STRING);
				$telefon = filter_var($_POST["telefon"],  FILTER_SANITIZE_STRING);
				$email   = filter_var($_POST["email"],    FILTER_SANITIZE_STRING);
				$sql = "UPDATE klient ";
				$sql .= "SET jmeno='".$jmeno."', ";
				$sql .= "prijmeni='".$prijmeni."', ";
				$sql .= "adresa='".$adresa."', ";
				$sql .= "telefon='".$telefon."', ";
				$sql .= "email='".$email."' ";
				$sql .= " WHERE r_cislo='".$rCislo."'";
				// Update informaci
				@mysql_query($sql, $db);
			}

			// Nacteni dat z databaze
			$query1="SELECT * FROM klient WHERE r_cislo = '".$_GET["setClientId"]."'";
			$result1 = @mysql_query($query1, $db); 
			$dataOut;
			while ($data = mysql_fetch_array($result1, MYSQL_ASSOC)) 
			{
				$dataOut = $data;
				// Vypsani informaci o klientovi
				?>
				<form action="?setClientId=<?php    echo $data["r_cislo"] ?>" method="post">
					<table>
						<tr><td>Jméno*</td><td><input type="text" value="<?php    echo $data["jmeno"] ?>" name="jmeno" required></td></tr>
						<tr><td>Příjmení*</td><td><input type="text" value="<?php echo $data["prijmeni"]?>" name="prijmeni" required></td></tr>
						<tr><td>Adresa</td><td><input type="text" value="<?php   echo $data["adresa"] ?>" name="adresa"></td></tr>
						<tr><td>Telefon</td><td><input type="number" value="<?php  echo $data["telefon"]?>" name="telefon"></td></tr>
						<tr><td>E-mail</td><td><input type="email" value="<?php  echo $data["email"]?>" name="email"></td></tr>
					</table>
					<input type="submit">
				</form>
				<?php
			}		
		}

		//client adding
		else if(isset($_GET["addClient"])) 
		{
			// Vypise formular pro pridani klienta - pote presmeruje na jeho clientId stranku
			if(isset($_POST["cislo_op"])) 
			{
				// Osetreni vstupnich hodnot
				// Ochrana proti SQL injekci
				$cislo_op= filter_var($_POST["cislo_op"], FILTER_SANITIZE_STRING);
				$rc=	   filter_var($_POST["rc"],       FILTER_SANITIZE_STRING);
				$jmeno=    filter_var($_POST["jmeno"],    FILTER_SANITIZE_STRING);
				$prijmeni= filter_var($_POST["prijmeni"], FILTER_SANITIZE_STRING);
				$adresa=   filter_var($_POST["adresa"],   FILTER_SANITIZE_STRING);
				$telefon=  filter_var($_POST["telefon"],  FILTER_SANITIZE_STRING);
				$email=    filter_var($_POST["email"],    FILTER_SANITIZE_STRING);
				// Update informaci
				$sql = "INSERT INTO klient (cislo_op, r_cislo, jmeno, prijmeni, adresa, telefon, email) VALUES ('{$cislo_op}', '{$rc}', '{$jmeno}', '{$prijmeni}', '{$adresa}', '{$telefon}', '{$email}')";

				if( mysql_query($sql, $db) == true) {
					header("Location: ?clientId={$rc}");
				}
			}	

			?>
				<form action="?addClient" method="post">
					<table>
						<tr><td>Cislo Op:*</td><td><input type="text" name="cislo_op" required></td></tr>
						<tr><td>Rodné číslo:* </td><td><input type="number" name="rc" required></td></tr>
						<tr><td>Jméno:*</td><td><input type="text" name="jmeno" required></td></tr>
						<tr><td>Příjmení:*</td><td><input type="text" name="prijmeni" required></td></tr>
						<tr><td>Adresa:</td><td><input type="text" name="adresa"></td></tr>
						<tr><td>Telefon:</td><td><input type="number" name="telefon"></td></tr>
						<tr><td>E-mail:</td><td><input type="email" name="email"></td></tr>
					</table>
					<input type="submit">
				</form>
			<?php
		}

		/*
		** Administratorska opravneni
		*/

		//list of workers
		else if(isset($_GET["listWorkers"]) and $_SESSION["role"] == "admin") 
		{
			// Vypise pracovniky v systemu s odkazem na workerId
			$query1="SELECT * FROM pracovnik_banky";
			$result1 = @mysql_query($query1, $db); 
			while ($data = mysql_fetch_array($result1, MYSQL_ASSOC)) {
				// Vypsani informaci o pracovnikovi
				?>
					<div class="zaznam">
						<?php echo "<a href=?workerId={$data["r_cislo"]}>{$data["jmeno"]} {$data["prijmeni"]}</a>, RČ: {$data["r_cislo"]}"; ?>
					</div>
				<?php
			}			
		}
		
		//worker information
		else if(isset($_GET["workerId"]) and $_SESSION["role"] == "admin") 
		{
			// Podobne jako clientId s tim rozdilem ze vypise informaci o pracovnikovi a jeho provedenych transakcich (ktere zadal)
			$query1="SELECT * FROM pracovnik_banky WHERE r_cislo = '".$_GET["workerId"]."'";
			$result1 = @mysql_query($query1, $db); 
			while ($data = mysql_fetch_array($result1, MYSQL_ASSOC)) 
			{
				// Vypsani informaci o pracovnikovy
				?>
				<table>
					<tr><td>ID</td><td><?php       echo $data["id_pracovnik"]?></td></tr>
					<tr><td>R. Č.</td><td><?php    echo $data["r_cislo"]?></td></tr>
					<tr><td>Jméno</td><td><?php    echo $data["jmeno"] ?></td></tr>
					<tr><td>Příjmení</td><td><?php echo $data["prijmeni"]?></td></tr>
					<tr><td>Adresa</td><td><?php   echo $data["adresa"] ?></td></tr>
					<tr><td>Telefon</td><td><?php  echo $data["telefon"]?></td></tr>
					<tr><td>E-mail</td><td><?php   echo $data["email"]?></td></tr>
					<tr><td>Login</td><td><?php    echo $data["login"]?></td></tr>
					<tr><td>Heslo</td><td><?php    echo $data["heslo"]?></td></tr>
					<tr><td>Pozicia</td><td><?php    echo $data["pozicia"]?></td></tr>
				</table>
				<?php echo "<a href=?setWorkerId={$data["r_cislo"]}>Editovat</a>"; ?>
				<?php echo "<a href=?deleteWorkerId={$data["r_cislo"]}>Zmazat</a>"; ?>
				<?php
			}		
			?>
			<?php
		}

		//zmazanie pracovnika
		else if(isset($_GET["deleteWorkerId"])) 
		{
			// Nacteni dat z databaze
			$query1="SELECT * FROM pracovnik_banky WHERE r_cislo = '".$_GET["deleteWorkerId"]."'";
			$result1 = @mysql_query($query1, $db); 
			while ($data = mysql_fetch_array($result1, MYSQL_ASSOC)) 
			{
				$sql = "DELETE FROM pracovnik_banky ";
				$sql .= "WHERE r_cislo='".$_GET["deleteWorkerId"]."'";
				@mysql_query($sql, $db);
			}		
		}

		//worker edit
		else if(isset($_GET["setWorkerId"]) and $_SESSION["role"] == "admin") 
		{
			// Vypise informace o pracovnikovi a moznost je upravovat
			if(isset($_POST["jmeno"])) 
			{
				$rCislo  = filter_var($_GET["setWorkerId"], FILTER_SANITIZE_STRING);
				$jmeno   = filter_var($_POST["jmeno"],     FILTER_SANITIZE_STRING);
				$prijmeni= filter_var($_POST["prijmeni"],  FILTER_SANITIZE_STRING);
				$adresa  = filter_var($_POST["adresa"],    FILTER_SANITIZE_STRING);
				$telefon = filter_var($_POST["telefon"],   FILTER_SANITIZE_STRING);
				$email   = filter_var($_POST["email"],     FILTER_SANITIZE_STRING);
				$login   = filter_var($_POST["login"],     FILTER_SANITIZE_STRING);
				$heslo   = filter_var($_POST["heslo"],     FILTER_SANITIZE_STRING);
				$pozicia = filter_var($_POST["pozicia"],   FILTER_SANITIZE_STRING);
				$sql = "UPDATE pracovnik_banky ";
				$sql .= "SET jmeno='".$jmeno."', ";
				$sql .= "prijmeni='".$prijmeni."', ";
				$sql .= "adresa='".$adresa."', ";
				$sql .= "telefon='".$telefon."', ";
				$sql .= "email='".$email."', ";
				$sql .= "login='".$login."', ";
				$sql .= "heslo='".$heslo."', ";
				$sql .= "pozicia='".$pozicia."' ";
				$sql .= " WHERE r_cislo='".$rCislo."'";

				// Update informaci
				@mysql_query($sql, $db);
			}

			// Nacteni dat z databaze
			$query1="SELECT * FROM pracovnik_banky WHERE r_cislo = '".$_GET["setWorkerId"]."'";
			$result1 = @mysql_query($query1, $db); 
			$dataOut;
			while ($data = mysql_fetch_array($result1, MYSQL_ASSOC)) 
			{
				$dataOut = $data;
				// Vypsani informaci o zamestnancovi
				?>
				<form action="?setWorkerId=<?php    echo $data["r_cislo"] ?>" method="post">
					<table>
						<tr><td>Jméno*</td><td><input type="text" value="<?php    echo $data["jmeno"] ?>" name="jmeno" required></td></tr>
						<tr><td>Příjmení*</td><td><input type="text" value="<?php echo $data["prijmeni"]?>" name="prijmeni" required></td></tr>
						<tr><td>Adresa</td><td><input type="text" value="<?php   echo $data["adresa"] ?>" name="adresa"></td></tr>
						<tr><td>Telefon</td><td><input type="text" value="<?php  echo $data["telefon"]?>" name="telefon"></td></tr>
						<tr><td>E-mail</td><td><input type="email" value="<?php  echo $data["email"]?>" name="email"></td></tr>
						<tr><td>Login*</td><td><input type="text" value="<?php    echo $data["login"]?>" name="login" required></td></tr>
						<tr><td>Heslo*</td><td><input type="password" value="<?php    echo $data["heslo"]?>" name="heslo" required></td></tr>
						<tr><td>Pozícia</td><td><input type="number" min="0" max="1" value="<?php  echo $data["pozicia"]?>" name="pozicia"></td></tr>
					</table>
					<input type="submit">
				</form>
				<?php
			}		
		}

		//adding worker
		else if(isset($_GET["addWorker"]) and $_SESSION["role"] == "admin") 
		{
			// Vypise formular, ktery prida noveho pracovnika do systemu -> po vytvoreni presmeruje na jeho workerId
			if(isset($_POST["id"])) 
			{
				// Osetreni vstupnich hodnot
				// Ochrana proti SQL injekci
				$id=	   filter_var($_POST["id"],       FILTER_SANITIZE_STRING);
				$rc=	   filter_var($_POST["rc"],       FILTER_SANITIZE_STRING);
				$jmeno=    filter_var($_POST["jmeno"],    FILTER_SANITIZE_STRING);
				$prijmeni= filter_var($_POST["prijmeni"], FILTER_SANITIZE_STRING);
				$adresa=   filter_var($_POST["adresa"],   FILTER_SANITIZE_STRING);
				$telefon=  filter_var($_POST["telefon"],  FILTER_SANITIZE_STRING);
				$email=    filter_var($_POST["email"],    FILTER_SANITIZE_STRING);
				$login=	   filter_var($_POST["login"],    FILTER_SANITIZE_STRING);
				$heslo=	   filter_var($_POST["heslo"],    FILTER_SANITIZE_STRING);
				$pozicia=  filter_var($_POST["pozicia"],  FILTER_SANITIZE_STRING);
				
				$sql = "INSERT INTO pracovnik_banky (id_pracovnik, r_cislo, jmeno, prijmeni, adresa, telefon, email, login, heslo, pozicia) VALUES ('{$id}', '{$rc}', '{$jmeno}', '{$prijmeni}', '{$adresa}', '{$telefon}', '{$email}', '{$login}', '{$heslo}', '{$pozicia}')";
				// Update informaci
				if( mysql_query($sql, $db) == true) {
				echo ("Zadali ste neunikatne ID55555.");
				header("Location: ?workerId={$rc}");
				}
				else {echo ("Zadali ste neunikátne ID. Nepodarilo sa vytvorit noveho pracovnika");}
			}	
			?>
				<form action="?addWorker" method="post">
					<table>
						<tr><td>ID:* </td><td><input type="number" name="id" required></td></tr>
						<tr><td>Rodné číslo:* </td><td><input type="number" name="rc" required></td></tr>
						<tr><td>Jméno:*</td><td><input type="text" name="jmeno" required></td></tr>
						<tr><td>Příjmení:*</td><td><input type="text" name="prijmeni" required></td></tr>
						<tr><td>Adresa:</td><td><input type="text" name="adresa"></td></tr>
						<tr><td>Telefon:</td><td><input type="number" name="telefon"></td></tr>
						<tr><td>E-mail:</td><td><input type="email" name="email"></td></tr>
						<tr><td>Login:*</td><td><input type="text" name="login" required=""></td></tr>
						<tr><td>Heslo:*</td><td><input type="password" name="heslo" required=""></td></tr>
						<tr><td>Pozicia:</td><td><input min="0" max="1" type="number" name="pozicia"></td></tr>
					</table>
					<input type="submit">
				</form>
			<?php
		}
?>

