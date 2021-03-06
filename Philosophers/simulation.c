/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bboulhan <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/14 12:33:20 by bboulhan          #+#    #+#             */
/*   Updated: 2022/05/14 12:33:32 by bboulhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_forks(int x, int y)
{
	if (x == y - 1)
		return (0);
	else
		return (x + 1);
}

void	sim_eat(t_philosof *p, struct timeval start, int *i)
{
	pthread_mutex_lock(p->sim->mutex[p->philosof_number]);
	printf("%d  %d has taken a fork\n",
		get_time(start), p->philosof_number + 1);
	pthread_mutex_lock(p->sim->mutex[check_forks(p->philosof_number,
			p->sim->number_of_philosophers)]);
	printf("%d  %d has taken a fork\n",
		get_time(start), p->philosof_number + 1);
	printf("%d  %d is eating\n",
		get_time(start), p->philosof_number + 1);
	(*i)++;
	gettimeofday(&p->death, NULL);
	ft_usleep(p->sim->time_to_eat);
	p->number_of_meals++;
	if (p->number_of_meals == p->sim->all_meals)
		p->sim->meals_left++;
	pthread_mutex_unlock(p->sim->mutex[check_forks(p->philosof_number,
			p->sim->number_of_philosophers)]);
	pthread_mutex_unlock(p->sim->mutex[p->philosof_number]);
}

void	*routine(void *arg)
{
	t_philosof		*p;
	struct timeval	start;
	int				i;

	i = 0;
	p = arg;
	gettimeofday(&start, NULL);
	gettimeofday(&p->death, NULL);
	if (p->philosof_number % 2 == 0)
		usleep(500);
	while (1)
	{
		sim_eat(p, start, &i);
		if (i > 0)
		{
			printf("%d  %d is sleeping\n",
				get_time(start), p->philosof_number + 1);
			ft_usleep(p->sim->time_to_sleep);
			i = 0;
			printf("%d  %d is thinking\n",
				get_time(start), p->philosof_number + 1);
		}
	}
	return (NULL);
}

int	creat_threads(struct timeval *start, pthread_t *philo,
	t_philosof **p, t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_philosophers)
	{
		if (pthread_create(&philo[i], NULL,
				&routine, (void *) p[i]) != 0)
			return (ft_error(2));
		if (i == 0)
			gettimeofday(start, NULL);
		i++;
	}
	return (0);
}

int	manager(pthread_t *philo, t_philosof **p, t_sim *sim)
{
	struct timeval	start;
	int				i;

	i = 0;
	if (creat_threads(&start, philo, p, sim))
		return (1);
	while (1)
	{
		if (get_time(p[i]->death) > sim->time_to_die)
		{
			pthread_mutex_lock(sim->dying);
			printf("%d  %d died\n", get_time(start),
				p[i]->philosof_number + 1);
			break ;
		}
		if (sim->meals_left == sim->number_of_philosophers
			&& sim->all_meals > 0)
			break ;
		i++;
		if (i >= sim->number_of_philosophers)
			i = 0;
	}
	free(philo);
	usleep(100);
	return (0);
}
