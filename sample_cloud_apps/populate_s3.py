import boto3
import boto.s3.connection

#prep the s3 client
def prep():
	ac_f = open('/home/liz/access/access_key',"r").read().strip()
	ac_s_f= open('/home/liz/access/secret_key',"r").read().strip()
	client = boto3.client('s3'
         	,aws_access_key_id=ac_f
		,aws_secret_access_key=ac_s_f
    		,region_name='us-east-1'
                ,use_ssl = False
                ,verify=False)
	return client

#add some random files to the bucket
def populate_s3(client,S3_BUCKET):
	for i in range(0,1):
		some_binary_data = b'Here we have some data'
		file = str('filename'+str(i)+'.txt')
		object = client.put_object(Body=some_binary_data,Bucket=S3_BUCKET, Key=file)
		print("putting " + file + "in " + S3_BUCKET)

if __name__ == '__main__':
	S3_BUCKET = 'diy-temp'
	client = prep()
	populate_s3(client,S3_BUCKET)
