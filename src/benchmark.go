package main

import (
	"math/rand"
	"time"
	"os"
	"github.com/tidwall/redbench"
)

var(
	hostname = "127.0.0.1"
	port     = 6379
	clients  = 50
	requests = 100000
	quiet    = false
	pipeline = 1
	csv      = false
	json     = false
	tests    = "PING,SET,GET,SEARCH,EVAL,llca_conc.CREATE,llca_conc.INSERT"
	redis    = false



)


func fillSingleOpts() *redbench.Options {
	singleopts := *redbench.DefaultOptions
	singleopts.CSV = csv
	singleopts.Clients = 1
	singleopts.Pipeline = 1
	singleopts.Quiet = quiet
	singleopts.Requests = 2
	singleopts.Stderr = os.Stderr
	singleopts.Stdout = os.Stdout
	return &singleopts
}


func fillOpts() *redbench.Options {
	opts := *redbench.DefaultOptions
	opts.CSV = csv
	opts.Clients = clients
	opts.Pipeline = pipeline
	opts.Quiet = quiet
	opts.Requests = requests
	opts.Stderr = os.Stderr
	opts.Stdout = os.Stdout
	return &opts
}

func main() {

	opts := fillOpts()
	singleopts := fillSingleOpts()
	
	
	redbench.Bench("SET", "127.0.0.1:6379", singleopts, nil, func(buf []byte) []byte {
		return redbench.AppendCommand(buf, "SET", "key:string", "val")
	})
	
	rand.Seed(time.Now().UnixNano())

	
	redbench.Bench("llca_type.INSERT","127.0.0.1:6379",opts,nil,func(buf []byte) []byte {
		return redbench.AppendCommand(buf,"llca_type.INSERT","key:abc","7")
	
	})
	
}