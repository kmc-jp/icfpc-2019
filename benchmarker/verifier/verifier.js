const puppeteer = require('puppeteer');
const readline = require('readline');
// Input from Stdin like {"desc": desc, "sol": sol}
// Or ARGV[2], ARGV[3]
const desc = process.argv[2];
const sol = process.argv[3];

(async () => {
  const browser = await puppeteer.launch({headless: true});
  const page = await browser.newPage();
  await page.goto('file:///'+__dirname+'/site/icfpcontest2019.github.io/solution_checker/index.html');
  page.on('console', msg => process.stderr.write('PAGE LOG:', msg.text()));

  //await task_input.uploadFile("/Users/admin/Documents/icfpc-2019/benchmarker/problems/part-1/prob-127.desc");
  //await solution_input.uploadFile("/Users/admin/Documents/icfpc-2019/benchmarker/results/2019-06-22T13:57:35+09:00__0dbc30c4__guchoku.o/prob-127.sol__10383");
  const verify = async (desc, sol) => {
    const task_input = await page.$('#submit_task');
    const solution_input = await page.$('#submit_solution');
    await task_input.uploadFile("");
    await solution_input.uploadFile("");
    await task_input.uploadFile(desc);
    await solution_input.uploadFile(sol);
    await page.waitForFunction(()=>{
      const c = document.querySelector('#output').textContent;
      console.log(c);
      return c.startsWith("Succ") || c.startsWith("Done") | c.startsWith("Fail") | c.startsWith("Cannot");
    });
    await page.click('#execute_solution');
    await page.waitForFunction(()=>{
      const c = document.querySelector('#output').textContent;
      console.log(c);
      const succ = c.startsWith("Success");
      const fail = c.startsWith("Fail") || c.startsWith("Cannot");
      return succ || fail;
    });
    const res = await page.$eval('#output', e=>e.textContent);
    if (res.startsWith("Success")) {
      const time = res.match(/(\d+) time units/)[1];
      process.stdout.write(JSON.stringify({status:"OK", "time":time})+"\n");
    } else {
      process.stdout.write(JSON.stringify({status:"NG", "msg":res})+"\n");
    }
  };
  if (desc) {
    await verify(desc, sol);
  } else {
    const rl = readline.createInterface({
      input: process.stdin,
      output: process.stdout,
      terminal: false
    });
    for await(const line of rl){
      const input = JSON.parse(line);
      await verify(input["desc"], input["sol"]);
    }
  }
  //await page.screenshot({path: 'screenshot.png'});
  await browser.close();
})();