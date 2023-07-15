$(document).ready(function() {
      $('#register').on('click',function(){
        $.ajax({
          url: "http://192.168.123.96:8000/decide",
          type: "GET",
          data:{
            text:"0"
          },
          success: function(response) {
              $('#replace').text("response")
          },
          error: function(error) {
              console.error(error);
          }
        });
      });
      $('#recognize').on('click',function(){
        $.ajax({
          url: "/decide",
          type: "GET",
          data:{
            text:"1"
          },
          success: function(response) {
              $('#replace').text("response")
          },
          error: function(error) {
              console.error(error);
          }
        });
      });
  });