const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<meta charset="UTF-8">
<style type="text/css">
  .button .put_water{
    background-color: #4CAF50;  /* Green */
    border: none;
    color: white;
    padding: 15px 32px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 16px;
  }
  
   .enable {
    background-color: #4CAF50;  /* Green */
    border: none;
    color: white;
    padding: 15px 32px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 16px;
  }
  
  .container {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(400px, 1fr));
  }
</style>

<body style="background-color: #f9e79f ">
  <center>
    <div>
      <h1>СЕРВЕР АВТОПОЛИВА v1.3</h1>
    </div>
    <h2>
            <div class="container">
               <div  class="timer" id="0" name="timer">
                  <span class="name" id="name" >Таймер 1: </span>
                  <button class="enable" style="background-color: #FF0000;" id="enable" value=0 onclick="enable_state()">Выключен</button><br>
                  Введите время полива <input type="time" class="time" name="watering_time" id="time" value="00:00"><br>
                  Период полива (дней) <input type="number" class="days" id="days" step="1" min="1" max="250"><br>
                  Длительность полива (секунд) <input type="number" class="seconds" id="seconds" step="1" min="1" max="60000"><br>
                    Канал: <select class="pin" id="pin">
                      <option value="0">A (0)</option>
                      <option value="2">B (2)</option>
                      <option value="12">C (12)</option>
                      <option value="13">D (13) </option>
                    </select><br>
                  Состояние: <span class="state" id="state">загружаем...</span><br>
                  <button class="button" onclick="send()">Сохранить</button><br><br>
                  <button class="put_water" id="put_water" onclick="put_water()">Полить</button><br><br>
               </div>
         
               <div  class="timer" id="1" name="timer">
                  <span class="name" id="name" >Таймер 2: </span>
                  <button class="enable" style="background-color: #FF0000;" id="enable" value=0 onclick="enable_state()">Выключен</button><br>
                  Введите время полива <input type="time" class="time" name="watering_time" id="time" value="00:00"><br>
                  Период полива (дней) <input type="number" class="days" id="days" step="1" min="1" max="250"><br>
                  Длительность полива (секунд) <input type="number" class="seconds" id="seconds" step="1" min="1" max="60000"><br>
                    Канал: <select class="pin">
                      <option value="0">A (0)</option>
                      <option value="2">B (2)</option>
                      <option value="12">C (12)</option>
                      <option value="13">D (13) </option>
                    </select><br>
                  Состояние: <span class="state" id="state">загружаем...</span><br>
                  <button class="button" onclick="send()">Сохранить</button><br><br>
                  <button class="put_water" id="put_water" onclick="put_water()">Полить</button><br><br>
               </div>
         
               <div  class="timer" id="2" name="timer">
                  <span class="name" id="name" >Таймер 3: </span>
                  <button class="enable" style="background-color: #FF0000;" id="enable" value=0 onclick="enable_state()">Выключен</button><br>
                  Введите время полива <input type="time" class="time" name="watering_time" id="time" value="00:00"><br>
                  Период полива (дней) <input type="number" class="days" id="days" step="1" min="1" max="250"><br>
                  Длительность полива (секунд) <input type="number" class="seconds" id="seconds" step="1" min="1" max="60000"><br>
          Канал: <select class="pin">
            <option value="0">A (0)</option>
            <option value="2">B (2)</option>
            <option value="12">C (12)</option>
            <option value="13">D (13) </option>
          </select><br>
                  Состояние: <span class="state" id="state">загружаем...</span><br>
                  <button class="button" onclick="send()">Сохранить</button><br><br>
                  <button class="put_water" id="put_water" onclick="put_water()">Полить</button><br><br>
               </div>
         
               <div  class="timer" id="3" name="timer">
                  <span class="name" id="name" >Таймер 4: </span>
                  <button class="enable" style="background-color: #FF0000;" id="enable" value=0 onclick="enable_state()">Выключен</button><br>
                  Введите время полива <input type="time" class="time" name="watering_time" id="time" value="00:00"><br>
                  Период полива (дней) <input type="number" class="days" id="days" step="1" min="1" max="250"><br>
                  Длительность полива (секунд) <input type="number" class="seconds" id="seconds" step="1" min="1" max="60000"><br>
          Канал: <select class="pin">
            <option value="0">A (0)</option>
            <option value="2">B (2)</option>
            <option value="12">C (12)</option>
            <option value="13">D (13) </option>
          </select><br>
                  Состояние: <span class="state" id="state">загружаем...</span><br>
                  <button class="button" onclick="send()">Сохранить</button><br><br>
                  <button class="put_water" id="put_water" onclick="put_water()">Полить</button><br><br>
               </div>
            </div>
            <details>
              Канал: <select class="test_pin">
                <option value="0">A (0)</option>
                <option value="2">B (2)</option>
                <option value="12">C (12)</option>
                <option value="13">D (13) </option>
              </select>
               <summary>Управление в ручную:</summary>
               <button class="put_water" onclick="start()">Старт</button>
               <button class="stop_water" onclick="stop()">Стоп</button>
               <br>
               <span class="test_state" id="state"></span>
         <br>
         <a href="log.txt">Лог файл</a><br>
         <a href="remove_file">Удалить лог файл</a><br>
         <a href="system_status">Системное состояние</a><br>
         <br>
     Инверсия выхода <button class="enable" style="background-color: #FF0000;" id="enable" value=0 onclick="enable_invert()">Выключен</button><br>
            </details>
         </h2>

         
    <script>
      started = false;
    
      function enable_invert() {
    var xhttp = new XMLHttpRequest();


        if (event.target.value == 0) {
          event.target.value = 1;
          event.target.style = "background-color: #4CAF50;"
          event.target.textContent = "Включен";
          xhttp.open("GET", "enable_invert?invert=1", true);
        } else {
          event.target.value = 0;
          event.target.style = "background-color: #FF0000;"
          event.target.textContent = "Выключен";
      xhttp.open("GET", "enable_invert?invert=0", true);
        };
    xhttp.send();
      }
    
      function enable_state() {
        if (event.target.value == 0) {
          event.target.value = 1;
          event.target.style = "background-color: #4CAF50;"
          event.target.textContent = "Включен";
        } else {
          event.target.value = 0;
          event.target.style = "background-color: #FF0000;"
          event.target.textContent = "Выключен";
        };
      }

      function clock_down(id) {
        let time = document.getElementsByClassName('seconds')[id].value; // Начало отсчёта
        const countdownElement = document.getElementsByClassName('state')[id];
        countdownElement.textContent = `Окончание полива через ${time} сек.`;
        const timer = setInterval(() => {
          time--;
          if (time > 0) {
            countdownElement.textContent = `Окончание полива через ${time} сек.`;
          } else {
      document.getElementsByClassName("put_water")[0].disabled = false;
      document.getElementsByClassName("put_water")[1].disabled = false;
      document.getElementsByClassName("put_water")[2].disabled = false;
      document.getElementsByClassName("put_water")[3].disabled = false;
      document.getElementsByClassName("put_water")[4].disabled = false;
            clearInterval(timer); // Остановка ожидания
            countdownElement.textContent = "Полив окончен"; // Сообщаем пользователю об этом
          }
        }, 1000);
      };

      function clock_up() {
        let time = 0; // Начало отсчёта
        const countdownElement = document.getElementsByClassName('test_state')[0];
        countdownElement.textContent = `Длительность полива ${time} сек.`;
        started = true;
        const timer = setInterval(() => {
          time++;
          if (started) {
            countdownElement.textContent = `Длительность полива ${time} сек.`;
          } else {
      document.getElementsByClassName("put_water")[0].disabled = false;
      document.getElementsByClassName("put_water")[1].disabled = false;
      document.getElementsByClassName("put_water")[2].disabled = false;
      document.getElementsByClassName("put_water")[3].disabled = false;
      document.getElementsByClassName("put_water")[4].disabled = false;
            clearInterval(timer); // Остановка ожидания
            //countdownElement.textContent = "Полив окончен"; // Сообщаем пользователю об этом
          }
        }, 1000);
      };

      function send() {
        let button = event.target;
        let timer = button.parentElement;
        let id = timer.id;
        console.log(id);
        let enable = timer.getElementsByClassName("enable")[0].value;
        let days = timer.getElementsByClassName("days")[0].value;
        let time = timer.getElementsByClassName("time")[0].value;
        let seconds = timer.getElementsByClassName("seconds")[0].value;
        let pin = timer.getElementsByClassName("pin")[0].value;
        //h1 = time[0];
        //h2 = time[1];
        //m1 = time[3];
        //m2 = time[4];
        var minutes = +time[3] * 10 + +time[4];
        var hours = +time[0] * 10 + +time[1];
        timer.getElementsByClassName("state")[0].textContent = "Записываем...";
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            timer.getElementsByClassName("state")[0].textContent = "Сохранено";
          }
        };
        xhttp.open("GET", "save_time?id=" + id + "&enable=" + enable + "&days=" + days + "&hours=" + hours + "&minutes=" + minutes + "&seconds=" + seconds + "&pin=" + pin, true);
        xhttp.send();
      }

      function put_water() {
        let button = event.target;
        let timer = button.parentElement;
        let id = timer.id;
        let pin = timer.getElementsByClassName("pin")[0].value;
        
        document.getElementsByClassName("put_water")[0].disabled = true;
        document.getElementsByClassName("put_water")[1].disabled = true;
        document.getElementsByClassName("put_water")[2].disabled = true;
        document.getElementsByClassName("put_water")[3].disabled = true;
        document.getElementsByClassName("put_water")[4].disabled = true;
    
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "put_water?id=" + id +"&pin=" + pin, true);
        xhttp.send();
        clock_down(id);
      }

      function start() {
        document.getElementsByClassName("put_water")[0].disabled = true;
        document.getElementsByClassName("put_water")[1].disabled = true;
        document.getElementsByClassName("put_water")[2].disabled = true;
        document.getElementsByClassName("put_water")[3].disabled = true;
        document.getElementsByClassName("put_water")[4].disabled = true;
        let pin = document.getElementsByClassName("test_pin")[0].value;
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "start?pin=" + pin, true);
        xhttp.send();
        clock_up();
      }

      function stop() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "stop", true);
        xhttp.send();
        started = false;
      }
      window.onload = getData();

      function multiplyNode(node, count, deep) {
        for (var i = 0, copy; i < count - 1; i++) {
          copy = node.cloneNode(deep);
          node.parentNode.insertBefore(copy, node);
          
          node.getElementsByClassName("name")[0].textContent = "Таймер " + String(i + 2) + ": ";
          node.id=i+1;
        }
      }

      function getData() {
        //multiplyNode(document.querySelector('.timer'), 2, true);
        var xhttp = new XMLHttpRequest();
        
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            let message = this.responseText;
            console.log(message);
            timers = message.split(';');
            for (id=0; id<4; id++) {
              console.log(timers[id]);
              numForSum = timers[id].split(',');
              document.getElementsByClassName("days")[id].value = numForSum[0];
              document.getElementsByClassName("time")[id].value = numForSum[1];
              document.getElementsByClassName("seconds")[id].value = numForSum[2];
              document.getElementsByClassName("enable")[id].value = numForSum[3];
              document.getElementsByClassName("pin")[id].value = numForSum[4];
              if (numForSum[3]==1){
                document.getElementsByClassName("enable")[id].style = "background-color: #4CAF50;"
                document.getElementsByClassName("enable")[id].textContent = "Включен";
              }
              document.getElementsByClassName("state")[id].textContent = "загружено";
            };
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
