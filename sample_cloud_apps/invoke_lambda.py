import boto3
from concurrent.futures import ThreadPoolExecutor, as_completed,wait
import time
import base64

def prep():
    ac_f = open('/home/liz/access/access_key',"r").read().strip()
    ac_s_f= open('/home/liz/access/secret_key',"r").read().strip()
    return ac_f, ac_s_f

def close(ac_f,ac_s_f):
   ac_f.close()
   ac_s_f.close()

def stripResponse(response):
    print(response.split('\n')[1].strip("ip: "))

def lambda_handler(client):
    global ac_s_f,ac_f,session
    #should i use a session?

    resp = client.invoke(
        FunctionName='Hello_World', #'+str(func),
        InvocationType='RequestResponse',
        LogType='Tail'
    )
    response = base64.b64decode(resp['LogResult'])
    #stripResponse(response)
    #print(response)


def execute(invokes,client):

    futures = []
    res= []

    miniFutures=[]
    for i in range(0,invokes):
        lambda_handler(client)
        print i

    print("done")

def par_execute(invokes,client):

    futures = []
    res= []

    miniFutures=[]
    with ThreadPoolExecutor(max_workers=1000) as executor:
        for i in range(0,invokes):
            futures.append(executor.submit(lambda_handler,client))
            #print i

    temp = wait(miniFutures)
    print("done")

if __name__ == '__main__':
    #session = boto3.session.Session()

    ac_f,ac_s_f = prep()
    client = boto3.client('lambda'
			,aws_access_key_id=ac_f
			,aws_secret_access_key=ac_s_f
    			,region_name='us-east-1'
                        #,use_ssl = False)
                        ,verify=False)

    total_funcs = 1
    lambdas=2
    #lambda_run_time = 10 # in seconds

    #for i in range(1,total_funcs+1):
    par_execute(lambdas,client)
