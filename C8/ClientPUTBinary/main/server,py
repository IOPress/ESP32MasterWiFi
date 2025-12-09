from http.server import HTTPServer, BaseHTTPRequestHandler
from io import BytesIO
temp=100.1234
class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.end_headers()
        global temp
        print(round(temp,2)) 
        self.wfile.write(str(round(temp,2)).encode("utf-8"))
    def do_PUT(self):
        global temp
        content_length = int(self.headers['Content-Length'])
        body = self.rfile.read(content_length)
        print(body)
        self.send_response(200)
        self.end_headers()
httpd = HTTPServer(('', 80), SimpleHTTPRequestHandler)
httpd.serve_forever()