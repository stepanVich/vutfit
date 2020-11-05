</div> <!-- .clear -->
<?php
	if(isset($_SESSION["isLogin"])) { 
?>
			<div class="footer">
				<a class = "logout-link" href="logout.php">Odhlásit se</a>	
				<div class="hodiny">Čas do odhlášení: <span class="hodiny-value"><span id="minutes">05</span>:<span id="seconds">00</span></span></div>
			</div>
			<script type="text/javascript">
			var minutesLabel = document.getElementById("minutes");
			var secondsLabel = document.getElementById("seconds");
			var totalSeconds = 5*60;
			setInterval(setTime, 1000);

			function setTime()
			{
				--totalSeconds;
				secondsLabel.innerHTML = pad(totalSeconds%60);
				minutesLabel.innerHTML = pad(parseInt(totalSeconds/60));
				if(totalSeconds <= 0) {
					// Presmerovani
					window.location = "logout.php";
				}
			}

			function pad(val)
			{
				var valString = val + "";
				if(valString.length < 2)
				{
					return "0" + valString;
				}
				else
				{
					return valString;
				}
			}
			</script>
<?php } ?>
		</div> <!-- .main -->
	</body>
</html>
