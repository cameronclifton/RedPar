package main

import (
	"os"
	"math/rand"
	"time"
	"strconv"
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

func random(min, max int) int {
    rand.Seed(time.Now().Unix())
    return rand.Intn(max - min) + min
}


func fillSingleOpts() *redbench.Options {
	singleopts := *redbench.DefaultOptions
	singleopts.CSV = csv
	singleopts.Clients = 1
	singleopts.Pipeline = 1
	singleopts.Quiet = quiet
	singleopts.Requests = 1
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

	redbench.Bench("llca_seq.create","127.0.0.1:6379",singleopts,nil,func(buf []byte) []byte {
		return redbench.AppendCommand(buf,"llca_seq.create","a")
	})

    redbench.Bench("llca_seq.insert","127.0.0.1:6379",opts,nil,func(buf []byte) []byte {
        num := random(1,100000)
		s := strconv.Itoa(num)
        return redbench.AppendCommand(buf,"llca_seq.insert","a",s)
    })

    redbench.Bench("llca_seq.mix","127.0.0.1:6379",opts,nil,func(buf []byte) []byte {
		myrand := random(1, 2)
		if myrand == 1{
			num := random(1,100000)
			s := strconv.Itoa(num)
			return redbench.AppendCommand(buf,"llca_seq.insert","a",s)
			} else {
			num := random(1,100000)
			s := strconv.Itoa(num)
			return redbench.AppendCommand(buf,"llca_seq.remove","a",s)
		}

	})



	redbench.Bench("llca_conc.create","127.0.0.1:6379",singleopts,nil,func(buf []byte) []byte {
		return redbench.AppendCommand(buf,"llca_conc.create","b")
	})

    redbench.Bench("llca_conc.insert","127.0.0.1:6379",opts,nil,func(buf []byte) []byte {
        num := random(1,100000)
		s := strconv.Itoa(num)
        return redbench.AppendCommand(buf,"llca_conc.insert","b",s)
    })

    redbench.Bench("llca_conc.mix","127.0.0.1:6379",opts,nil,func(buf []byte) []byte {
		myrand := random(1, 2)
		if myrand == 1{
			num := random(1,100000)
			s := strconv.Itoa(num)
			return redbench.AppendCommand(buf,"llca_conc.insert","b",s)
			} else {
			num := random(1,100000)
			s := strconv.Itoa(num)
			return redbench.AppendCommand(buf,"llca_conc.remove","b",s)
		}

	})

}
