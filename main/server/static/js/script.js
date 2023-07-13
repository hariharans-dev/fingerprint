$(document).ready(function() {
    setInterval(function() {
      $.ajax({
        url: "/fetchdata",
        type: "GET",
        success: function(response) {
            $('#replace').text("response")
        },
        error: function(error) {
            console.error(error);
        }
      });
    }, 1000);
});

