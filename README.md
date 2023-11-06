# ExpressionWhizz
/var/local/scottycheck/isse-09/grader: line 192: 761679 Aborted                 (core dumped) ./a.out 1>&$outfile


Leaked 0 memory blocks

Correctness: 0 out of 56
Memory leaks: 20 out of 20
Traceback (most recent call last):
  File "/var/local/scottycheck/isse-09/ew_interactive_test.py", line 108, in <module>
    run_tests(sys.argv[1])
  File "/var/local/scottycheck/isse-09/ew_interactive_test.py", line 100, in run_tests
    run_one_test(child, test[0], re.escape(test[1]))
  File "/var/local/scottycheck/isse-09/ew_interactive_test.py", line 71, in run_one_test
    child.expect(prompt_re)
  File "/usr/lib/python3/dist-packages/pexpect/spawnbase.py", line 343, in expect
    return self.expect_list(compiled_pattern_list,
  File "/usr/lib/python3/dist-packages/pexpect/spawnbase.py", line 372, in expect_list
    return exp.expect_loop(timeout)
  File "/usr/lib/python3/dist-packages/pexpect/expect.py", line 179, in expect_loop
    return self.eof(e)
  File "/usr/lib/python3/dist-packages/pexpect/expect.py", line 122, in eof
    raise exc
pexpect.exceptions.EOF: End Of File (EOF). Exception style platform.
<pexpect.pty_spawn.spawn object at 0x7faa49803d00>
command: ./interact
args: [b'./interact']
buffer (last 100 chars): ''
before (last 100 chars): "\r\ninteract: clist.c:55: CL_free: Assertion `list' failed.\r\n"
after: <class 'pexpect.exceptions.EOF'>
match: None
match_index: None
exitstatus: None
flag_eof: True
pid: 761714
child_fd: 5
closed: False
timeout: 1
delimiter: <class 'pexpect.exceptions.EOF'>
logfile: None
logfile_read: <_io.TextIOWrapper name='ew_interactive_test.log' mode='w' encoding='UTF-8'>
logfile_send: None
maxread: 2000
ignorecase: False
searchwindowsize: None
delaybeforesend: 0.05
delayafterclose: 0.1
delayafterterminate: 0.1
searcher: searcher_re:
    0: re.compile('[Ee]xpr\\? *')



Input '3 + (2*': Expected 'Unexpected\ token\ \(end\)'