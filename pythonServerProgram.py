import cv2
import numpy as np
import pyzbar.pyzbar as pyzbar
import requests
import urllib.request

font = cv2.FONT_HERSHEY_PLAIN

url = 'http://192.168.110.247/'
cv2.namedWindow("live transmission", cv2.WINDOW_AUTOSIZE)

prev = ""
pres = ""
while True:
    img_resp = urllib.request.urlopen(url + 'cam-hi.jpg')
    imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
    frame = cv2.imdecode(imgnp, -1)

    decodedObjects = pyzbar.decode(frame)
    for obj in decodedObjects:
        pres = obj.data.decode('utf-8')  # Convert bytes to string
        if prev != pres:  # Hanya kirim jika URL berbeda dengan sebelumnya
            print("Type:", obj.type)
            print("Data: ", pres)
            
            # Kirim data ke layanan IFTTT
            event_name = 'qrCode'
            key = 'bTqsHemd3m9Aog37VEfUljNzJ4RzKBEedVIoJ-bpevV'  # Ganti dengan kunci webhook IFTTT Anda
            url = f'https://maker.ifttt.com/trigger/{event_name}/with/key/{key}'
            data = {'value1': pres}
            response = requests.post(url, json=data)
            if response.status_code == 200:
                print("Data sent to IFTTT successfully")
            else:
                print("Failed to send data to IFTTT")

            prev = pres

        cv2.putText(frame, str(obj.data), (50, 50), font, 2, (255, 0, 0), 3)

    cv2.imshow("live transmission", frame)

    key = cv2.waitKey(1)
    if key == 27:
        break

cv2.destroyAllWindows()
