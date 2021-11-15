import requests
import sys

def DownloadFile(url, filepath):
    with open(filepath, 'wb') as f:
        response = requests.get(url, stream=True)
        total = response.headers.get('content-length')

        if total is None:
            f.write(response.content)
        else:
            total = int(total)
            downloaded = 0
            for chunk in response.iter_content(chunk_size=max(int(total/1000), 1024 * 1024)):
                if chunk:
                    f.write(chunk)
                    downloaded += len(chunk)
                    percentage = (downloaded / total) * 100
                    sys.stdout.write('\rDownloading ... {:.2f}%'.format(percentage))
                    sys.stdout.flush()
    sys.stdout.write('\n')

# From github.com/TheCherno/Hazel
def YesOrNo():
    while True:
        reply = str(input('[Y/N]: ')).lower().strip()
        if reply[:1] == 'y':
            return True
        if reply[:1] == 'n':
            return False
