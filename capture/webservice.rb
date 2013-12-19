# myapp.rb

#require 'IO'
require 'sinatra'
require 'rubygems'

set :bind, '0.0.0.0'
set :static, true

get '/' do
'<!DOCTYPE html>
<html>
<head>
    <title>WallHax</title>
<meta name="viewport" content="initial-scale=1, maximum-scale=1">
 <style>
        html,body,img,input {
            display: block;
            margin: auto;
	    margin-top:50px;
	    width: 300px;
        }
    </style>
<script>
function go() {
document.querySelector("form").submit();
}
</script>
</head>
<body>

<form method="POST" action="/" enctype="multipart/form-data">
    <input type="file" name="picture"/>
    <img src="go.jpeg" onclick="go();"/>
</form>

</body>
</html>'
end

post '/' do
  File.open('capture.jpg', "w") do |f|
    f.write(params['picture'][:tempfile].read)
 end
f = IO.popen('sh pipeline.sh')
p f.readlines
f.close

redirect to('/') 
end
