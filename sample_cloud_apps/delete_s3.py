import boto3

#prep the s3 client
def prep():
    	ac_f = open('/home/liz/access/access_key',"r").read().strip()
	ac_s_f= open('/home/liz/access/secret_key',"r").read().strip()
	client = boto3.client('s3'
         	,aws_access_key_id=ac_f
		,aws_secret_access_key=ac_s_f
    		,region_name='us-east-1')

	return client

#https://harrymoreno.com/2017/04/24/How-to-fill-and-empty-an-s3-bucket-with-python.html
#deleting all objects of bucket
def delete_s3(client,S3_BUCKET):
	response = client.list_objects_v2(Bucket=S3_BUCKET)
	if 'Contents' in response:
    		for item in response['Contents']:
      			print('deleting file', item['Key'])
      			client.delete_object(Bucket=S3_BUCKET, Key=item['Key'])
      			while response['KeyCount'] == 1000:
        			response = client.list_objects_v2(
          				Bucket=S3_BUCKET,
          				StartAfter=response['Contents'][0]['Key'],
        			)
        			for item in response['Contents']:
          				print('deleting file', item['Key'])
          				client.delete_object(Bucket=S3_BUCKET, Key=item['Key'])

if __name__ == '__main__':
	S3_BUCKET = 'diy-temp'
	client = prep()
	delete_s3(client,S3_BUCKET)
