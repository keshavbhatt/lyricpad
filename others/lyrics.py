#web scrapping code made by Rishabh Bhardwaj
# follow me at http://www.knoobypie.com/about-me/
# folow me on linkedin @https://www.linkedin.com/in/rishabh-bhardwaj-791903171/
# github https://github.com/rishabh3354

# <<<<<<<<-pre-requestie->>>>>>>>>>>>>>>>
# install the following python module:
# pip install requests
# pip install bs4
# output will be in html format, Recommended to save file in html format


import requests
from bs4 import BeautifulSoup
import sys
import json

query = sys.argv[1:]
query_str = " ".join(query)
# print(query_str)
# query_str = input("enter song name\n")
# query_str = "fadded"

url = "https://search.azlyrics.com/search.php?q=" + query_str

headers = {'User-Agent': 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0'}

try:
    response = requests.get(url, timeout=10, headers=headers)


except requests.ConnectionError as err:
    print("<<<<<< PLEASE CHECK YOUR INTERNET CONNECTION >>>>")
except requests.Timeout as err:
    print("OOPS!! Timeout Error")
except requests.RequestException as err:
    print("OOPS!! UNEXPECTED Error")

else:

    data = response.text

    soup = BeautifulSoup(data, 'lxml')
    boldtag = soup.find_all("td", {"class": "text-left visitedlyr"})

    tags = soup.find_all('a')

    l2 = []
    for x in tags:
        l2.append(x.get('href'))
    song_url = []
    for y in l2:
        if "www.azlyrics.com/lyrics/" in y:
            song_url.append(y)

    # print(boldtag)
    boldtag=list(boldtag)
    if len(song_url)!=0:


        sl=[]
        for c in boldtag:
            c=str(c)
            # print(c.split())
            if 'href="https://www.azlyrics.com/lyrics/' in c:
                # print("true")
                sl.append(c)
        # print(sl[0])

        song=[]
        artist=[]

        for itm in range(len(sl)):

            # print(sl[0])
            kk=sl[itm].split("<b>")
            # print(kk[1])
            pp=kk[1].split("</b")
            # print(pp[0])
            song.append(pp[0])



            kk2=sl[itm].split("<b>")
            # print(kk2[2])
            pp2=kk2[2].split("</b>")
            # print(pp2[0])
            artist.append(pp2[0])

        # print(song)
        # print(artist)

        # print(song_url)   # links

        song_title = []
        song_id = []
        artist_name = []

        finaldata = []
        dictdata = {}

        for counter in range(0, len(song_url)):
            dictdata['song_id'] = str(counter)
            dictdata['song_name'] = str(song[counter])
            dictdata['song_artist'] = str(artist[counter])
            dictdata['song_url'] = str(song_url[counter])
            finaldata.append(dictdata.copy())
        r = json.dumps(finaldata)
        print(r)
        sys.stdout.flush()


    else:
        print("lYRICS NOT FOUND")
