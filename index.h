const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<meta charset="UTF-8">
<style type="text/css">
  .button {
    background-color: #4CAF50;  /* Green */
    border: none;
    color: white;
    padding: 15px 32px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 16px;
  }
</style>

<body style="background-color: #f9e79f ">
  <center>
    <div>
      <h1>СЕРВЕР АВТОПОЛИВА v1.0</h1>
      <br>
      <button class="button" onclick="send()">Сохранить</button>
    </div>
    <div>
      <h2>
         Введите время полива <input type="time" name="watering_time" id="time" value='00:00'></span><br><br>
         Период полива (дней) <input type="number" id="days" step="1" min="1" max="250"></span><br><br>
         Длительность полива (секунд) <input type="number" id="seconds" step="1" min="1" max="60000"></span><br><br>
         Состояние: <span id="state">NA</span><br><br>
      </h2>
      <button class="button" onclick="put_water()">Полить</button>
    </div>
    <script>
      function send() {
        days = document.getElementById("days").value;
        time = document.getElementById("time").value;
        seconds = document.getElementById("seconds").value;
        h1 = time[0];
        h2 = time[1];
        m1 = time[3];
        m2 = time[4];
        var minutes = +m1 * 10 + +m2;
        var hours = +h1 * 10 + +h2;
        document.getElementById("state").innerHTML = "Записываем...";
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("state").innerHTML = "Сохранено";
          }
        };
        xhttp.open("GET", "save_time?days=" + days + "&hours=" + hours + "&minutes=" + minutes + "&seconds=" + seconds, true);
        xhttp.send();
      }

      function put_water() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "put_water", true);
        xhttp.send();
      }
      window.onload = getData();

      function getData() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            let numForSum = this.responseText;
            numForSum = numForSum.split(',');
            document.getElementById("days").value = numForSum[0];
            document.getElementById("time").value = numForSum[1];
            document.getElementById("seconds").value = numForSum[2];
          }
        };
        xhttp.open("GET", "read", true);
        xhttp.send();
      }
    </script>
  </center>
</body>
</html>
)=====";
