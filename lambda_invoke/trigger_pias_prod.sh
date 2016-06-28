#!/bin/bash

INPUT=failed_ids.csv
OLDIFS=$IFS
IFS=,
[ ! -f $INPUT ] && { echo "$INPUT file not found"; exit 99; }
while read dmksId absNr
do
	echo "DMKS ID : $dmksId"
	echo "ABS Number : $absNr"

	echo "invoking lambda"

	echo "PAYLOAD: {\"dmks_id\":\"$dmksId\",\"ApplicationResponse\":{\"Type\":\"whatever\",\"ApplicationId\": \"$absNr\",\"Status\": \"whatever2\"},\"sendSummaryPdf\": true}"

	aws lambda invoke \
	--invocation-type RequestResponse \
	--function-name vwfs-dmks-euw1-lambda-pias:test \
	--region eu-west-1 \
	--log-type Tail \
	--payload "{\"dmks_id\":\"$dmksId\",\"ApplicationResponse\":{\"Type\":\"whatever\",\"ApplicationId\": \"$absNr\",\"Status\": \"whatever2\"},\"sendSummaryPdf\": true}" \
	outputfile.txt

	cat outputfile.txt

	echo -e "\n"


done < $INPUT
IFS=$OLDIFS
