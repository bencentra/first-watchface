(function() {

  var $submit = $('#submitButton'),
      $outerColorPicker = $('#outerColorPicker'),
      $innerColorPicker = $('#innerColorPicker'),
      $timeFormatCheckbox = $('#timeFormatCheckbox');

  function loadOptions() {
    if (localStorage.outerColor) {
      $outerColorPicker.val(localStorage.outerColor);
    }
    if (localStorage.innerColor) {
      $innerColorPicker.val(localStorage.innerColor);
    }
    if (localStorage.twentyFourHourFormat) {
      $timeFormatCheckbox[0].checked = localStorage.twentyFourHourFormat === 'true';
    }
  }

  function getAndStoreConfigData() {
    var options = {
      outerColor: $outerColorPicker.val(),
      innerColor: $innerColorPicker.val(),
      twentyFourHourFormat: $timeFormatCheckbox[0].checked
    };

    localStorage.outerColor = options.outerColor;
    localStorage.innerColor = options.innerColor;
    localStorage.twentyFourHourFormat = options.twentyFourHourFormat;

    console.log('Got options: ' + JSON.stringify(options));
    return options;
  }

  function getQueryParam(variable, defaultValue) {
    var query = location.search.substring(1);
    var vars = query.split('&');
    for (var i = 0; i < vars.length; i++) {
      var pair = vars[i].split('=');
      if (pair[0] === variable) {
        return decodeURIComponent(pair[1]);
      }
    }
    return defaultValue || false;
  }
  
  $submit.on('click', function() {
    console.log('Submit');
    var return_to = getQueryParam('return_to', 'pebblejs://close#');
    document.location = return_to + encodeURIComponent(JSON.stringify(getAndStoreConfigData()));
  });

  loadOptions();
  
})();

