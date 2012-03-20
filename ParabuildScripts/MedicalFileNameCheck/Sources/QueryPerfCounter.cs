using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.ComponentModel;

namespace FileNameConvention
{
  class QueryPerfCounter
  {
    [DllImport("KERNEL32")]
    private static extern bool QueryPerformanceCounter(out long lpPerformanceCount);

    [DllImport("Kernel32.dll")]
    private static extern bool QueryPerformanceFrequency(out long lpFrequency);

    private long start;
    private long stop;
    private long frequency;
    private int iterations = 1;
    Decimal multiplier = new Decimal(1.0e9);

    public QueryPerfCounter()
    {
      if (QueryPerformanceFrequency(out frequency) == false)
      {
        // Frequency not supported
        throw new Win32Exception();
      }
    }

    /// <summary>
    /// Start the system times
    /// </summary>
    public void Start()
    {
      QueryPerformanceCounter(out start);
    }

    /// <summary>
    /// Stop the system timer
    /// </summary>
    public void Stop()
    {
      QueryPerformanceCounter(out stop);
    }

    /// <summary>
    /// Set the number of iteration of the code monitored.
    /// </summary>
    /// <param name="it">Number of iterations (> 0)</param>
    public void SetIterations(int it)
    {
      iterations = it <= 0 ? 1 : it;
    }

    /// <summary>
    /// Set the time multiplier for the time measuring; the value must be > 0.
    /// Default value is nanoseconds: 10e9
    /// </summary>
    /// <param name="mult"></param>
    public void SetMultiplier(double mult)
    {
      if (mult <= 0)
      {
        return;
      }
      multiplier = Convert.ToDecimal(mult);
    }

    /// <summary>
    /// Calculate the time interval in nanosecond 
    /// (default value for multiplier: 1.0e9)
    /// </summary>
    /// <returns>Time interval for each iteration of the code stub monitored</returns>
    public double Duration()
    {
      return ((((double)(stop - start) * (double)multiplier) / (double)frequency) / iterations);
    }
  }
}
