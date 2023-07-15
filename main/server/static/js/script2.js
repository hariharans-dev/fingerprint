$(document).ready(function() {
    setInterval(function() {
      $.ajax({
        url: "/recgfetch",
        type: "GET",
        success: function(response) {
            $('#rep').text(response)
        },
        error: function(error) {
            console.error(error);
        }
      });
    }, 1000);
});